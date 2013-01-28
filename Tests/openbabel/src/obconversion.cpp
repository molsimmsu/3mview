#include <iostream>
#include <fstream>

#include <openbabel/obconversion.h>
#include <openbabel/mol.h>
using namespace OpenBabel;

int main(int argc,char **argv)
{
    OBConversion obconversion;
    if (!obconversion.SetInFormat("PDB")) {
        std::cout << "Error setting format " << std::endl;
        return 1;
    }
    
    OBMol mol;
    
    std::ifstream in;
    in.open(argv[1]);
    if (!in.is_open()) {
        std::cout << "Error loading file " << argv[1] << std::endl;
        return 1;
    }

    if (!obconversion.Read(&mol, &in)) {
        std::cout << "Error reading from stream" << std::endl;
        return 1;
    }
    
    std::cout << "Molecular Weight: " << mol.GetMolWt() << std::endl;

    return 0;
}
