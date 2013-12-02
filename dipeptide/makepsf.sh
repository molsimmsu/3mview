#!/bin/bash

pdb=$1
psf=$2
top=$3
pgn="temp.pgn"

if [ -e "$pgn" ]
then
    rm "$pgn"
fi

echo "
package require psfgen
topology $top

segment MAIN {
pdb $pdb
first ACE
last CT3
}

coordpdb $pdb MAIN
guesscoord
writepdb $pdb
writepsf $psf
exit
" | psfgen
