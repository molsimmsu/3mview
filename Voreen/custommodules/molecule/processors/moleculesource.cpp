#include "moleculesource.h"

#include "../utils/stridereader.h"
#include "../utils/moleculeio.h"

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
        Molecule* mol = MoleculeIO::read(MoleculeURL(inputFile_.get()));
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
