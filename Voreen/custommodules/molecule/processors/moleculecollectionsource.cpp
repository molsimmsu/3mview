#include "moleculecollectionsource.h"

#include "../utils/stridereader.h"

#include "openbabel/obconversion.h"
#include "openbabel/mol.h"
using namespace OpenBabel;

#include <fstream>
#include <string>

MoleculeCollectionSource::MoleculeCollectionSource()
  : Processor()
  , outport_(Port::OUTPORT, "molecule", "Molecule Output")
  , inputFile_("inputFile", "Input file", "Load PDB", VoreenApplication::app()->getUserDataPath(), "*.pdb")
  , loadMolecule_("loadMolecule", "Load Molecule")
{
    addPort(outport_);
    
    addProperty(inputFile_);
    addProperty(loadMolecule_);
    
    loadMolecule_.onChange(CallMemberAction<MoleculeCollectionSource>(this, &MoleculeCollectionSource::readMolecule));
    
	// Create empty data to make this outport valid. Take ownership is true because
	// we want the data to be automatically deleted when replaced at the next setData() call
    outport_.setData(new MoleculeCollection(), true);
}

MoleculeCollection* MoleculeCollectionSource::getMoleculeCollection() {
    return outport_.getWritableData();
}

void MoleculeCollectionSource::readMolecule() {
    try {
		// Load new molecule
        Molecule* mol = loadMoleculeFromFile(inputFile_.get());
        tgtAssert(mol, "null pointer to mol returned (exception expected) at MoleculeCollectionSource::readMolecule()");
        
        getMoleculeCollection()->add(mol);
        outport_.invalidatePort();
    }
    catch (VoreenException& e) {
        LERROR(e.what());
    }
    catch (...) {
        LERROR("Error at MoleculeCollectionSource::readMolecule()");
    }
}

Molecule* MoleculeCollectionSource::loadMoleculeFromFile(const std::string& filename)
    throw (VoreenException)
{
    std::ifstream stream;
    stream.open(filename.c_str());
    if (stream.fail())
        throw VoreenException("Failed to open file for reading: " + filename);

    std::string molFormat("PDB");

    LWARNING("Start loading");

    OBConversion conv;
    if (!conv.SetInFormat(molFormat.c_str()))
        throw VoreenException("Failed to set input format for reading molecule: " + molFormat);
        
    LWARNING("Start conversion");
        
    OBMol mol;
    if (!conv.Read(&mol, &stream))
        throw VoreenException("Failed to read molecule from file: " + filename);
        
    LWARNING("Start STRIDE");
    
    //StrideReader stride(filename.c_str());
    
    LWARNING("Loading successful");
    
    //return new Molecule(mol, stride.getStructure());
    return new Molecule(mol);
}
