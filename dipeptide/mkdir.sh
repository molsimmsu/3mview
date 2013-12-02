#!/bin/bash

dir=$1

if [ ! -e "$dir" ]
then
    mkdir "$dir"
    cd "$dir"
    mkdir "seq"
    mkdir "pdb"
    mkdir "psf"
    mkdir "dih"
    mkdir "his"
    mkdir "png"
    cd ..
fi
