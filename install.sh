#!/usr/bin/env bash

# See README for instructions, as well as 
# Jira ticket - https://offworld-ai.atlassian.net/browse/AAGSB-1207
# for more context

# install linux kernel headers
sudo apt install -y linux-headers-$(uname -r)

# install libncurses-dev (needed if you wish to compile the apcilib CLI examples)
sudo apt install -y libncurses-dev

# compile 
make

# install/load
sudo make install
