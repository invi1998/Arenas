FROM python:3.12-slim

RUN apt-get update

RUN apt-get install -y curl

RUN curl -fsSL https://get.docker.com | sh

COPY . /

WORKDIR /coordinator

RUN pip3 install flask

ENTRYPOINT [ "python", "coordinator.py" ]


