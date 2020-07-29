# Genetic Algorithm with Machine Learning Feature Selection
Human Activity Recognition (HAR) attempts to acknowledge a person's behaviour through a series of impressions about themselves and the environment around them. Recognition may be determined by focus on the knowledge gathered from specific sources such as environment or body-worn sensors. It is a challenging problem because there is no clear analytical way to relate the sensor data to specific actions in a general way. It is technically challenging because of the massive volume of sensor data collected and the classical use of hand-crafted features and heuristics from these data in developing predictive models.

The Human Activity Recognition database was built from recordings of 30 study participants performing activities of daily living (ADL) while carrying a waist positioned smartphone with inertial sensors embedded in it. The goal is to identify the actions under one of the six activities that have been carried out by the human which are walking, walking upstairs, walking downstairs, sitting, standing, laying. The need for specific field expertise becomes the challenge of existing solutions. Therefore, to find an experimental planning that can optimize the algorithm for this problem, we implement the feature selection using Genetic Algorithm (GA).


# Build Docker flask API 
Step 1: Build the dDocker file
```
docker build -t quek/flask_svm .
```

Step 2: run the docker file
```
docker run -d -p 5000:5000 quek/flask_svm
```

# CURL on Visual Studio 2019
Solution explorer -> Right Click Project -> Properties -> 

Step 1: C/C++ -> Additional Include Directories
```buildoutcfg
C:\Users\user\Documents\UTeM Year 2 Sem 2\BITI3143 - Evolutionary Computing\Mini_Project\curl-7.70.0\x32 Release\include\curl
```
You can find the files in the [curl-7.70.0](curl-7.70.0)

Step 2: linker -> Input -> Additional Dependencies
```buildoutcfg
Normaliz.lib;Ws2_32.lib;Wldap32.lib;Crypt32.lib;advapi32.lib;C:\Users\user\Documents\UTeM Year 2 Sem 2\BITI3143 - Evolutionary Computing\Mini_Project\curl-7.70.0\x64 Debug\lib\libcurl_a_debug.lib
```
2
# Hyperparameter Tuning
+ change the Number of GENE
+ Change the CURL_POST_FIELD_SIZE 
