#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/../../openbabel-2.3.2/lib
export BABEL_LIBDIR=${PWD}/../../openbabel-2.3.2/lib
export BABEL_DATADIR=${PWD}/../../openbabel-2.3.2/data

./voreenve
