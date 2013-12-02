#!/bin/bash

f="$1-$2"
r="results"

echo "Running $f"
#./build.sh "$1" "$2"
./makeconf.sh "$r/pdb/$f.pdb" "$r/psf/$f.psf" "$f.dat" "$f.conf"

grep -Po 'TCL: 1PHIPSY \K.*' "$f.dat" > "$r/dih/$f-1.dih"
grep -Po 'TCL: 2PHIPSY \K.*' "$f.dat" > "$r/dih/$f-2.dih"
rm "$f.dat"

python makehist.py "$r/dih/$f-1.dih" "$r/his/$f-1.his"
python makehist.py "$r/dih/$f-2.dih" "$r/his/$f-2.his"

./makegnu.sh "$r/his/$f-1.his" "$r/png/$f-1.png" "$f-1" "$f.gnu"
./makegnu.sh "$r/his/$f-2.his" "$r/png/$f-2.png" "$f-2" "$f.gnu"

rm -f output.*
