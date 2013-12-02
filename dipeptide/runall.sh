#!/bin/bash

dir="results"

./mkdir.sh "$dir"

for fn in PDB/*; do
    f1=${fn:4:3}
    for fn in PDB/*; do
        f2=${fn:4:3}
        
        ./run.sh "$f1" "$f2"
    done
done
