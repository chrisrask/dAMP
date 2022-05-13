# getting base image ubuntu
FROM ubuntu

ARG DEBIAN_FRONTEND=noninteractive

MAINTAINER Christopher Rask <christopher.rask@afry.com>

COPY setup.sh /tmp/setup.sh
RUN DEBIAN_FRONTEND=noninteractive /tmp/setup.sh

#The below two are to ensure that correct pytest/pytest-html is installed
RUN adduser --disabled-password --gecos "" user
USER user