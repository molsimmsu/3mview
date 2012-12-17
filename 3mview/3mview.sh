#! /usr/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/ost/stage/lib:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/ost/stage/lib64:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/openbabel/lib:
./3mview
