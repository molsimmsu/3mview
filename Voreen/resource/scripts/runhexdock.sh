#!/bin/bash
macrosname=macros.mac
rm -f $macrosname
numresults=$3
path=`pwd`
receptor=$path"/"`echo $1 | sed -e 's/.pdb$//g'`
ligand=$path"/"`echo $2 | sed -e 's/.pdb$//g'`
rm -f $receptor\_no_hetatm.pdb
rm -f $ligand\_no_hetatm.pdb
cat $receptor.pdb | grep ^ATOM >> $receptor\_no_hetatm.pdb
cat $ligand.pdb | grep ^ATOM >> $ligand\_no_hetatm.pdb

echo "open_receptor " $receptor\_no_hetatm.pdb >> $macrosname
echo "open_ligand " $ligand\_no_hetatm.pdb >> $macrosname
echo "activate_docking" >> $macrosname

for((i=1;i<=$numresults;i++))
do
    echo "view_docking_solution " $i >> $macrosname
    echo "save_receptor "$receptor\_dock_$i\_.pdb >> $macrosname
    echo "save_ligand "$ligand\_dock_$i\_.pdb >> $macrosname
done
log=`date +'%m-%d-%G_%H-%M'`\_hex_log.txt
if which hex >/dev/null
then
    echo "Running_hex"
    hex -batch -e $macrosname >> $log
else
    echo "Hex_is_not_foud"
fi

