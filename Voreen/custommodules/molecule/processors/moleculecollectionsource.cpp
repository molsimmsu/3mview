#include "moleculecollectionsource.h"

#include "voreen/core/processors/processorwidget.h"

#include "../utils/stridereader.h"
#include "../utils/moleculereader.h"

#include "openbabel/obconversion.h"
#include "openbabel/mol.h"
using namespace OpenBabel;

#include <fstream>
#include <string>

const std::string MoleculeCollectionSource::loggerCat_("3mview.molecule.MoleculeCollectionSource");

MoleculeCollectionSource::MoleculeCollectionSource()
  : Processor()
  , outport_(Port::OUTPORT, "moleculecollection", "Molecule Collection Output")
  , coProcessorPort_(Port::OUTPORT, "coProcessor", "Molecule Collection Co-processor")
  , inputFile_("inputFile", "Input file", "Load PDB", VoreenApplication::app()->getUserDataPath(), "*.pdb")
  , loadMolecule_("loadMolecule", "Load Molecule")
  , moleculeURLlist_("moleculeURLlist_", "Molecule list", std::vector<std::string>())
{
    addPort(outport_);
    addPort(coProcessorPort_);
    
    addProperty(inputFile_);
    addProperty(loadMolecule_);
    addProperty(moleculeURLlist_);
    
    loadMolecule_.onChange(CallMemberAction<MoleculeCollectionSource>(this, &MoleculeCollectionSource::readMolecule));
    
	// Create empty data to make this outport valid. Take ownership is true because
	// we want the data to be automatically deleted when replaced at the next setData() call
    outport_.setData(new MoleculeCollection(), true);
}

void MoleculeCollectionSource::initialize() throw (tgt::Exception) {
    LWARNING("MoleculeCollectionSource::initialize()");
    Processor::initialize();
    /*
    moleculeURLlist_.loadMolecules(false, true);
    outport_.setData(moleculeURLlist_.getMolecules(true), true);

    if (getProcessorWidget())
        getProcessorWidget()->updateFromProcessor();*/
}

void MoleculeCollectionSource::invalidate(int inv) {
    LWARNING("MoleculeCollectionSource::invalidate()");
    /*outport_.setData(moleculeURLlist_.getMolecules(true), true);
    
    const std::vector<CoProcessorPort*>& coProcessorOutports =  getCoProcessorOutports();
    
    for (size_t i = 0; i < coProcessorOutports.size(); ++i) {
        const std::vector<const Port*> connectedPorts = coProcessorOutports[i]->getConnected();
        
        for (size_t j = 0; j < connectedPorts.size(); ++j) {
            LWARNING("Port");
            Processor* processor = connectedPorts[i]->getProcessor();
            try {
                //dynamic_cast<DensityMapCoProcessor*>(processor)->updateSelection();
            }
            catch (...) {}
        }
    }
    */
    Processor::invalidate(inv);
}

void MoleculeCollectionSource::load(const std::string& path) {
    try {
        Molecule* mol = MoleculeReader::read(path);
        tgtAssert(mol, "null pointer to mol returned (exception expected) at MoleculeCollectionSource::readMolecule()");

        getMoleculeCollection()->add(mol);
        LWARNING(mol->getOrigin().getPath());
        moleculeURLlist_.addMolecule(mol, false, true);
        outport_.invalidatePort();
    }
    catch (VoreenException& e) {
        LERROR(e.what());
    }
    catch (...) {
        LERROR("Error at MoleculeCollectionSource::load()");
    }
}


MoleculeCollection* MoleculeCollectionSource::getMoleculeCollection() {
    return outport_.getWritableData(); //moleculeURLlist_.getMolecules(false);
}

MoleculeCollection* MoleculeCollectionSource::getSelectedMoleculeCollection() {
    return outport_.getWritableData(); //moleculeURLlist_.getMolecules(true);
}

void MoleculeCollectionSource::applyTransformation(tgt::vec3 offset, tgt::mat4 matrix) {
    //LWARNING("MoleculeCollectionSource::applyTransformation()");
    MoleculeCollection* mc = outport_.getWritableData();
    for (size_t i = 0; i < mc->size(); i++) {
        Molecule* mol = mc->at(i);
        matrix[0][3] = offset[0];
        matrix[1][3] = offset[1];
        matrix[2][3] = offset[2];
        mol->transform(matrix);
    }
}

void MoleculeCollectionSource::readMolecule() {
    load(inputFile_.get());
}

