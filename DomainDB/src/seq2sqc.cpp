#include "Molecule.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
if (argc < 3) {cout << "usage: seq sqc" << endl; return 1;}

Molecule molecule;
molecule.readSEQ(argv[1]);
molecule.writeSQC(argv[2]);
}
