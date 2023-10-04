#!/bin/bash

apt install -y software-properties-common
add-apt-repository ppa:ubuntu-toolchain-r/test

apt update -y

apt install -y gcc-13 g++-13 -y
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13 --slave /usr/bin/g++ g++ /usr/bin/g++-13
gcc --version
