#!/bin/bash

apt update -y && apt upgrade -y

apt install -y g++ make git binutils cmake libssl-dev libboost-system-dev zlib1g-dev build-essential curl libcurl4-openssl-dev
apt install -y qtcreator qtbase5-dev qt5-qmake clang
apt install -y mysql-server libqt5sql5-mysql 
snap install mysql-workbench-community
