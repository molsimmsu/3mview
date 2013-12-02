#!/bin/bash

his=$1
png=$2
title=$3
out=$4

if [ -e "$file" ]
then
    rm "$file"
fi

echo "
set terminal png
set output \"$png\"
set size square
set title '$title'
set xlabel 'Phi'
set ylabel 'Psi'
set xrange[-180:180]
set yrange[-180:180]
set pm3d map
set palette defined (0 \"white\", 0.002 \"blue\", 0.005 \"cyan\", 0.015 \"green\", 0.02 \"yellow\", 0.03 \"orange\", 0.035 \"orange\")
splot '$his' notitle
" >> "$out"

gnuplot "$out"
rm "$out"
