#!/bin/bash
if [ -f /proc/cpuinfo ]; then
	NUM_CORES=`cat /proc/cpuinfo | grep processor | wc -l`
else
	if [ -x /usr/sbin/system_profiler ]; then
		NUM_CORES=`/usr/sbin/system_profiler -detailLevel full SPHardwareDataType | awk '/Total Number .+ Cores/ {print $5};'`
	else
		NUM_CORES=1
	fi
fi
INITIAL="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $INITIAL
cd 3mview/ost
cmake . -DOPTIMIZE=1
make -j$NUM_CORES
cd $INITIAL/3mview/openbabel
cmake .
make -j$NUM_CORES
cd $INITIAL/3mview
if [ `uname` = 'Darwin' ]; then
	SUFFIX=-spec macx-g++
fi
qmake $SUFFIX 3mview.pro
make -j$NUM_CORES
