FROM frolvlad/alpine-python-machinelearning:latest
RUN pip install --upgrade pip

# Initialized working directory
WORKDIR /app

# copy our code to the docker
COPY . /app

RUN pip install -r requirements.txt

EXPOSE 4000

ENTRYPOINT ['python']

CMD ['app.py']


