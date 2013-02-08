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
    
	// Create empty data to make this outport valid. Take ownership is true because
	// we want the data to be automatically deleted when replaced at the next setData() call
    outport_.setData(new Molecule(), true);
}

void MoleculeSource::readMolecule() {
    try {
		// Load new molecule
        Molecule* mol = loadMoleculeFromFile(inputFile_.get());
        tgtAssert(mol, "null pointer to mol returned (exception expected) at MoleculeSource::readMolecule()");
        
        // Delete old data and set new
        outport_.setData(mol);
    }
    catch (VoreenException& e) {
        LERROR(e.what());
    }
    catch (...) {
        LERROR("Error at MoleculeSource::readMolecule()");
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
        
    OBMol mol;
    if (!conv.Read(&mol, &stream))
        throw VoreenException("Failed to read molecule from file: " + filename);
    
    StrideReader stride(filename.c_str());
    
    return new Molecule(mol, stride.getStructure());
}
