#include "Molecule.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
if (argc < 4) {cout << "usage: xyz2pdb xyz pdb sqc" << endl; return 1;}

Molecule molecule;
molecule.readXYZ(argv[1]);
molecule.writePDB(argv[2]);
molecule.writeSQC(argv[3]);
}
