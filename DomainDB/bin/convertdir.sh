#!/bin/sh

cp $1 $2
cd $2;
a=`ls *.pdb`
echo "Applying " $1 " to " $2
for b in $a; do
echo $b "..."
c=`echo $b | cut -d. -f1`
$1 $c >> ../db.dat
done
rm $1

###
###	Script gets a program name and directory as an input 
###  and runs the program for each file of the direcroty
###     example: sh convertdir.sh ./main ./1
