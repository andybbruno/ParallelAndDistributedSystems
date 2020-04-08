### start from current ubuntu image
FROM ubuntu:eoan
MAINTAINER marcod

### install sudo
RUN apt-get update && \
    apt-get -y install sudo
    
### install base programming tools
RUN apt update \
    && apt install -y gcc-9 g++-9 \
    && apt install -y git subversion \
    && apt install -y vim \
    && apt install -y cmake \
    && apt install -y autoconf \
    && apt install -y libreadline-dev \
    && apt install -y software-properties-common \
    && apt install -y htop
### make g++-9 default g++ compiler
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 100
### install FastFlow
RUN cd /usr/local/ && git clone https://github.com/fastflow/fastflow.git
RUN cd /usr/local/include && ln -s ../fastflow/ff . 
### install GRPPi
RUN cd /usr/local && git clone https://github.com/arcosuc3m/grppi.git
RUN cd /usr/local/include && ln -s ../grppi/include/grppi 
### install rplsh
RUN cd /usr/local && git clone https://github.com/Murray1991/rplsh.git
RUN cd /usr/local/rplsh && ./install.sh
### set up service ssh
RUN apt install -y ssh 
RUN service ssh start
### add tbb
RUN apt install -y libtbb-dev
### add rsync
RUN apt install -y rsync
### fix user
RUN useradd -ms /bin/bash spm2020user

RUN sudo apt-get install -y libboost-all-dev

USER spm2020user

WORKDIR /home/spm2020user
