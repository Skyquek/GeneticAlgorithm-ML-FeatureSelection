from flask import Flask, jsonify, request
import pandas as pd
from sklearn.utils import shuffle
from sklearn import preprocessing
from sklearn.preprocessing import StandardScaler
import numpy as np
import pandas as pd
import random
from sklearn.utils import shuffle
from sklearn.svm import SVC
from sklearn.metrics import confusion_matrix, classification_report, f1_score
from sklearn.model_selection import cross_val_score, GridSearchCV

app = Flask(__name__)

encoder = preprocessing.LabelEncoder()

GENE = 200


def data_preprocessing(trainData, testData, chromosome_feature):
    # Seperating Predictors and Outcome values from train and test sets
    X_train = pd.DataFrame(trainData.drop(['Activity', 'subject'], axis=1))
    X_train = X_train.iloc[:, 0:200]
    Y_train_label = trainData.Activity.values.astype(object)

    X_test = pd.DataFrame(testData.drop(['Activity', 'subject'], axis=1))
    X_test = X_test.iloc[:, 0: 200]
    Y_test_label = testData.Activity.values.astype(object)

    print("Before X_train shape: {}, X_test shape: {}".format(X_train.shape, X_test.shape))

    # chromosome_list = [random.randint(0, 1) for x in range(1, 562)]
    X_train, X_test, total_non_activated, total_activated = featureSelection(chromosome_feature, X_train, X_test)

    columns_name = X_train.columns.tolist()

    print("After X_train shape: {}, X_test shape: {}".format(X_train.shape, X_test.shape))

    try:
        # encoding train labels
        encoder.fit(Y_train_label)
        Y_train = encoder.transform(Y_train_label)

        # encoding test labels
        encoder.fit(Y_test_label)
        Y_test = encoder.transform(Y_test_label)

        scaler = StandardScaler()
        X_train_scaled = scaler.fit_transform(X_train)
        X_test_scaled = scaler.transform(X_test)

        # print(X_train_scaled.shape)  # Row: 7352, Column: 561

        return X_train_scaled, Y_train, X_test_scaled, Y_test, Y_test_label, columns_name, total_non_activated, total_activated
    except:
        X_train_scaled = 0
        Y_train = 0
        X_test_scaled = 0
        Y_test = 0

        X_train_scaled, Y_train, X_test_scaled, Y_test, Y_test_label, columns_name, total_non_activated, total_activated


def featureSelection(chromosome_list, X_train, X_test):
    """
    :param chromosome_list: Python list on the binary chromosome value. Use shape=(,GENE)
    :param X_train: Total columns of training set
    :param X_test: Total columns of testing set
    :return: Selected columns from the chromosome

    In this experiment, all the gene represented the column or the attribute of the dataset.
    """

    print("chromosome list:", chromosome_list)
    print("length chromosome list:", len(chromosome_list))

    dropped_list = []

    for gene in range(0, len(chromosome_list)):
        if chromosome_list[gene] == 0:
            dropped_list.append(gene)

    print("dropped list", dropped_list)
    # Drop the feature that show 0
    X_train.drop(X_train.columns[dropped_list], axis=1, inplace=True)
    X_test.drop(X_test.columns[dropped_list], axis=1, inplace=True)

    # print("Total Activated Feature: {}, Total Non-Activated Feature: {}".format(GENE-len(dropped_list),
    # len(dropped_list)))

    total_non_activated = len(dropped_list)
    total_activated = GENE - len(dropped_list)

    return X_train, X_test, total_non_activated, total_activated


@app.route("/", methods=['GET', 'POST'])
def home():
    if request.method == 'POST':
        keys = request.form.keys()
        chromosome_list = []
        attribute_counter = 0

        for chromosome in keys:
            for alelle in range(0, GENE):
                if int(chromosome[alelle]) == 1:
                    attribute_counter += 1

                chromosome_list.append(int(chromosome[alelle]))

        # If list all have less than 2 attributes
        if attribute_counter < 2:
            return str(0)

        trainData = shuffle(pd.read_csv('static/datasets/train.csv'))
        testData = shuffle(pd.read_csv('static/datasets/test.csv'))

        X_train_scaled, Y_train, X_test_scaled, Y_test, Y_test_label, columns_name, total_non_activated, total_activated = data_preprocessing(
            trainData, testData, chromosome_list)

        try:
            svm_model = SVC(kernel='rbf')
            svm_model.fit(X_train_scaled, Y_train)
            Y_pred = svm_model.predict(X_test_scaled)
            Y_pred_label = list(encoder.inverse_transform(Y_pred))

            classification_result = classification_report(Y_test_label, Y_pred_label, output_dict=True)

            f1_list = []

            for val in classification_result.values():
                if type(val) is dict:
                    f1_list.append(val['f1-score'])

            f1_mean = np.mean(np.array(f1_list))

            # result = {
            #     "fitness": f1_mean,
            #     "column_name": columns_name,
            #     "total_0": total_non_activated,
            #     "total_1": total_activated
            # }
            #
            # return jsonify(result)

            return str(f1_mean)
        except:
            return str(0)
    else:
        return "Hello Get"


if __name__ == "__main__":
    app.run()
