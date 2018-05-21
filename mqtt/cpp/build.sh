#!/bin/sh

mkdir -p build
cd build

cmake ..
make
./riks-samples-mqtt-client
