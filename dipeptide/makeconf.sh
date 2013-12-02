#!/bin/bash

pdb=$1
psf=$2
dat=$3
conf=$4

if [ -e "$dat" ]
then
    rm "$dat"
fi

if [ -e "$conf" ]
then
    rm "$conf"
fi

echo "
structure	$psf
coordinates	$pdb
" >> "$conf"

cat template.conf >> "$conf"

echo "Running NAMD..."
namd2 "$conf" >> "$dat"
rm "$conf"
