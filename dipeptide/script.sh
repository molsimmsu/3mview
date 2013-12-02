#!/bin/bash
root="results"
ang="-60 -30"

rm -r -f "$root"
mkdir "$root"

for fn in PDB/*; do
    f1=${fn:4:3}
    for fn in PDB/*; do
        f2=${fn:4:3}
        
        ./run.sh "$f1" "$f2"
    done
done
