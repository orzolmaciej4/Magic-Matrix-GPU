#!/bin/bash

cd ~
wget https://github.com/numactl/numactl/releases/download/v2.0.16/numactl-2.0.16.tar.gz
tar -xf numactl-2.0.16.tar.gz
cd numactl-2.0.16

./configure --prefix=$HOME
make
make install
echo "Numactl v2.0.16 has been installed to the home directory"

echo "Executing numactl -H on the login-node and saving the output to numactl.out..."
"$HOME/numactl-2.0.16/numactl" -H > "$HOME/numactl.out"
echo "The output of numactl -H has been documented in numactl.out"
