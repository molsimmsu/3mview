#include "moleculecoprocessor.h"

namespace voreen {

MoleculeCoProcessor::MoleculeCoProcessor()
    : Processor()
    , inport_(Port::INPORT, "MoleculeSource", "Molecule Source")
{
    addPort(inport_);
}

Processor* MoleculeCoProcessor::create() const {
    return new MoleculeCoProcessor();
}

MoleculeCollectionSource* MoleculeCoProcessor::getSourceProcessor() const {
    std::vector<Processor*> processors = inport_.getConnectedProcessors();
    if (processors.size() == 0) {
        LWARNING("Processors.size() is 0 at MoleculeCoProcessor::getSourceProcessor()");
        return 0;
    }
    
    Processor* processor = processors[0];
    
    if (processor == 0) {
        LWARNING("Processor is NULL at MoleculeCoProcessor::getSourceProcessor()");
        return 0;
    }
    
    if (typeid(*processor) != typeid(MoleculeCollectionSource)) {
        LWARNING("Processor is not a MoleculeCollectionSource at MoleculeCoProcessor::getSourceProcessor()");
        return 0;
    }
    
    return static_cast<MoleculeCollectionSource*>(processor);
}

const MoleculeCollection* MoleculeCoProcessor::getInputMoleculeCollection() const {
    MoleculeCollectionSource* source = getSourceProcessor();
    if (source == 0) {
        LWARNING("Source is NULL at MoleculeCoProcessor::getInputVolumeCollection()");
        return 0;
    }
    
    return source->getSelectedMoleculeCollection();
}

} // namespace
