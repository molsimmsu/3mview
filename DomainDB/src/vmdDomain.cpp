#include "VMD.h"

int main(int argc, char** argv)
{
VMD vmd;

vector<string> mol1;
mol1.push_back(string("1.pdb"));
mol1.push_back(string("2.pdb"));

vector<string> mol2;
mol2.push_back(string("3.pdb"));

VMDMolecules mols;
mols.push_back(mol1);
mols.push_back(mol2);

vmd.loadMolecules(mols);
}
