#!/bin/bash

set -e

fxsdk build-fx
cp fxNET.g1a "$1/fxNET.g1a"
sync