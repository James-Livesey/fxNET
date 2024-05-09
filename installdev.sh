#!/bin/bash

rm -rf installdev
mkdir -p installdev

pushd installdev
    wget https://duktape.org/duktape-2.7.0.tar.xz -O duktape.tar.xz
    tar -xf duktape.tar.xz
    mv duktape-2.7.0 duktape
popd