#!/bin/bash
ang="-60 -30"

dir=$1
f1=$2
f2=$3

file="$dir/$f1-$f2.seq"

if [ -e "$file" ]
then
    rm "$file"
fi

echo "$f1 $ang" >> "$file"
echo "$f2 $ang" >> "$file"
