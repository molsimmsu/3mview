#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/ost/stage/lib:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/ost/stage/lib64:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PWD}/openbabel/lib:
if [ -f ./3mview ]; then
	./3mview
else
	open 3mview.app
fi
