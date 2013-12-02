#!/bin/bash

f="$1-$2"
r="results"

./buildseq.sh "$r/seq" "$1" "$2"
./seq2pdb.pl "$r/seq/$f.seq" "$r/pdb/$f.pdb"
./makepsf.sh "$r/pdb/$f.pdb" "$r/psf/$f.psf" top_all36_prot.rtf
