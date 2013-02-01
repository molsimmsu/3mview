#include "moleculesource.h"

#include "../utils/stridereader.h"

#include "openbabel/obconversion.h"
#include "openbabel/mol.h"
using namespace OpenBabel;

#include <fstream>
#include <string>

MoleculeSource::MoleculeSource()
  : Processor()
  , outport_(Port::OUTPORT, "molecule", "Molecule Output")
  , inputFile_("inputFile", "Input file", "Load PDB", VoreenApplication::app()->getUserDataPath(), "*.pdb")
  , loadMolecule_("loadMolecule", "Load Molecule")
  , clearMolecule_("clearMolecule", "Clear Molecule")
{
    addPort(outport_);
    
    addProperty(inputFile_);
    addProperty(loadMolecule_);
    addProperty(clearMolecule_);
    
    loadMolecule_.onChange(CallMemberAction<MoleculeSource>(this, &MoleculeSource::readMolecule));
    
    Molecule* molecule = new Molecule();
    
    outport_.setData(molecule);
}

void MoleculeSource::readMolecule() {
    try {
        Molecule* molecule = loadMoleculeFromFile(inputFile_.get());
        tgtAssert(molecule, "null pointer returned (exception expected)");
        outport_.setData(molecule);
    }
    catch (VoreenException& e) {
        LERROR(e.what());
    }
}

Molecule* MoleculeSource::loadMoleculeFromFile(const std::string& filename)
    throw (VoreenException)
{
    std::ifstream stream;
    stream.open(filename.c_str());
    if (stream.fail())
        throw VoreenException("Failed to open file for reading: " + filename);

    std::string molFormat("PDB");

    OBConversion conv;
    if (!conv.SetInFormat(molFormat.c_str()))
        throw VoreenException("Failed to set input format for reading molecule: " + molFormat);
        
    OBMol* mol = new OBMol();
    if (!conv.Read(mol, &stream))
        throw VoreenException("Failed to read molecule from file: " + filename);
    
    StrideReader stride(filename.c_str());
    
    return new Molecule(mol, stride.getStructure());
}
