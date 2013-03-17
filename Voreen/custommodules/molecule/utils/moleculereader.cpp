#include "moleculereader.h"

#include "../utils/stridereader.h"

#include "openbabel/obconversion.h"
#include "openbabel/mol.h"
using namespace OpenBabel;

#include <fstream>
#include <string>

const std::string MoleculeReader::loggerCat_ = "3mview.MoleculeReader";

Molecule* MoleculeReader::read(const MoleculeURL& url) {
    return read(url.getPath());
}

Molecule* MoleculeReader::read(const std::string& filename) 
    throw (FileException) 
{
    std::ifstream stream;
    stream.open(filename.c_str());
    if (stream.fail())
        throw FileException("Failed to open file for reading ", filename);

    std::string molFormat("PDB");

    LWARNING("Start loading");

    OBConversion conv;
    if (!conv.SetInFormat(molFormat.c_str()))
        throw FileException("Failed to set input format for reading molecule", filename);
        
    LWARNING("Start conversion");
        
    OBMol mol;
    if (!conv.Read(&mol, &stream))
        throw FileException("Failed to read molecule from file", filename);
        
    //LWARNING("Start STRIDE");
    
    // FIXME StrideReader stride(filename.c_str());
    
    LWARNING("Loading successful");
    
    //return new Molecule(mol, stride.getStructure());
    Molecule* result = new Molecule(mol);
    result->setOrigin(VolumeURL(filename));
    return result;
}