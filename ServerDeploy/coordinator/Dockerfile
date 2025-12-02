# FROM python:3.12-slim

# RUN apt-get update

# RUN apt-get install -y curl

# RUN curl -fsSL https://get.docker.com | sh

# COPY . /

# WORKDIR /coordinator

# RUN pip3 install flask

# ENTRYPOINT [ "python", "coordinator.py" ]


FROM python:3.12-slim

RUN apt-get update && apt-get install -y curl

# 安装Docker
RUN curl -fsSL https://get.docker.com | sh

# 确保docker在PATH中
ENV PATH="/usr/bin:${PATH}"

# 测试docker是否可用
RUN docker --version || echo "Docker installation test failed"

COPY . /

WORKDIR /coordinator

RUN pip3 install flask

ENTRYPOINT [ "python", "coordinator.py" ]


