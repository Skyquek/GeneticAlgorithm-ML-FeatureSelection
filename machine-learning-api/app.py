from flask import Flask, jsonify
import pandas as pd
from sklearn.utils import shuffle
from sklearn import preprocessing
from sklearn.preprocessing import StandardScaler
import numpy as np
import pandas as pd
from sklearn.utils import shuffle
from sklearn.svm import SVC
from sklearn.metrics import confusion_matrix, classification_report, f1_score
from sklearn.model_selection import cross_val_score, GridSearchCV

app = Flask(__name__)

encoder = preprocessing.LabelEncoder()


def data_preprocessing(trainData, testData):
    # Seperating Predictors and Outcome values from train and test sets
    X_train = pd.DataFrame(trainData.drop(['Activity', 'subject'], axis=1))
    Y_train_label = trainData.Activity.values.astype(object)

    X_test = pd.DataFrame(testData.drop(['Activity', 'subject'], axis=1))
    Y_test_label = testData.Activity.values.astype(object)

    # encoding train labels
    encoder.fit(Y_train_label)
    Y_train = encoder.transform(Y_train_label)

    # encoding test labels
    encoder.fit(Y_test_label)
    Y_test = encoder.transform(Y_test_label)

    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)

    print(X_train_scaled)

    return X_train_scaled, Y_train, X_test_scaled, Y_test, Y_test_label


@app.route("/")
def home():
    print("Start the Application")
    trainData = shuffle(pd.read_csv('static/datasets/train.csv'))
    testData = shuffle(pd.read_csv('static/datasets/test.csv'))

    X_train_scaled, Y_train, X_test_scaled, Y_test, Y_test_label = data_preprocessing(trainData, testData)

    print("[DEBUG] Done Preprocessing")

    svm_model = SVC(kernel='rbf')
    svm_model.fit(X_train_scaled, Y_train)
    Y_pred = svm_model.predict(X_test_scaled)
    Y_pred_label = list(encoder.inverse_transform(Y_pred))

    classification_result = classification_report(Y_test_label, Y_pred_label, output_dict=True)

    f1_list = []

    for val in classification_result.values():
        if type(val) is dict:
            print(val)
            f1_list.append(val['f1-score'])

    f1_mean = np.mean(np.array(f1_list))

    result = {"fitness": f1_mean}
    return jsonify(result)


if __name__ == "__main__":
    app.run()
