#!/bin/bash  -i
set -e
set -x
set -m

apt_install="apt-get install -y --no-install-recommends"
pip_install="pip3 install --upgrade"

apt-get update

echo -e "Start Git Installation"
$apt_install \
 git \
 gitk \
 git-review

echo -e "Git Install Success"

echo -e "Start Python Installation"
$apt_install \
 python3 \
 python3-dev \
 python3-pip \
 python3-setuptools \
 python-setuptools \
 python3-tk \

echo -e "Install Build Essentials and Make"
$apt_install \
 build-essential \
 make

echo -e "Install Finished!"

# last !
rm -rf /var/lib/apt/lists/*
python3 -m pip install --upgrade pip
$pip_install pytest
$pip_install pytest-html
$pip_install pycodestyle
$pip_install pylint
$pip_install autopep8