FROM frolvlad/alpine-python-machinelearning:latest
RUN pip install --upgrade pip

# copy our code to the docker
COPY machine-learning-api/ /application
WORKDIR /application

RUN pip install -r requirements.txt

EXPOSE 5000

CMD ["python","app.py"]


