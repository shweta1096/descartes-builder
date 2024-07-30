#
#FROM horovod/horovod:sha-9b42eda
FROM pytorch/pytorch:1.13.1-cuda11.6-cudnn8-runtime
LABEL description="DesCartes Builder Dockerfile"

COPY requirements.txt /root
WORKDIR /root

# Python setup
RUN python -m pip install --upgrade pip
RUN pip3 install -r requirements.txt

# Dev setup
RUN apt-get update && \
    apt-get install -y --no-install-recommends make git vim
RUN git config --global --add safe.directory /root/shared/

#EXPOSE 8888/tcp
ENV SHELL /bin/bash



ENTRYPOINT ["/bin/bash"]
