#!/bin/bash

dir="results"

./mkdir.sh "$dir"

i=1

for fn in PDB/*; do
    if [ "$i" -ge "$1" -a "$i" -le "$2" ]
    then
        f1=${fn:4:3}
        for fn in PDB/*; do
            f2=${fn:4:3}
            
            echo "Running $f1 $f2"
            ./run.sh "$f1" "$f2"
        done
    fi
    ((i++))
done
