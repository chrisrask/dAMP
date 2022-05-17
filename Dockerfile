FROM ubuntu

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends\
    git \
    gitk \
    git-review \
    python3 \
    python3-dev \
    python3-pip \
    python3-setuptools \
    python-setuptools \
    python3-tk \
    build-essential \
    make \
&& rm -rf /var/lib/apt/lists/*

RUN pip3 install --upgrade\
 pytest \
 pytest-html \
 pycodestyle \
 pylint \
 autopep8

RUN adduser --disabled-password --gecos "" user
USER user