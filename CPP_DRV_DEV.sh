#!/bin/sh
sudo apt-get install build-essential gdb dpkg-dev -y
sudo apt-get install linux-headers-$(uname -r) -y
sudo apt-get install linux-source -y

