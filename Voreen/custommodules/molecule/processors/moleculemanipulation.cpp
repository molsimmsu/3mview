#include "moleculemanipulation.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"

namespace voreen {

MoleculeManipulation::MoleculeManipulation()
    : ManipulationBase()
    , moleculeSelection_("moleculeSelection", "Molecule selection")
{
    //LINFO("ENTER MoleculeManipulation::MoleculeManipulation()");
	addProperty(moleculeSelection_);
	//LINFO("EXIT MoleculeManipulation::MoleculeManipulation()");
}

MoleculeManipulation::~MoleculeManipulation() {
    
}

Processor* MoleculeManipulation::create() const {
    //std::cout << "MoleculeManipulation::create()" << std::endl;
    return new MoleculeManipulation();
}

void MoleculeManipulation::applyTransformation(tgt::vec3 offset, tgt::mat4 matrix) {
    //LINFO("MoleculeManipulation::applyTransformation()");
    ManipulationBase::applyTransformation(offset, matrix);
    
        const MoleculeCollection* collection = moleculeSelection_.getSelectedMolecules();
        if (collection == 0 || collection->size() == 0) return;
        
        for (size_t i = 0; i < collection->size(); i++) {
            Molecule* molecule = collection->at(i);
            
            if (typeid(*molecule) != typeid(Molecule)) {
                LWARNING("Base class is not an instance of Molecule");
                continue;
            }
            
            tgt::mat4 transform = molecule->getTransformationMatrix();
            static_cast<Molecule*>(molecule)->setTransformationMatrix(transform * matrix);
        }
        
        Processor* processor = getSourceProcessor();
        if (processor != 0)
            processor->getPort("moleculecollection")->invalidatePort();
}

void MoleculeManipulation::invalidate(int inv) {
    //LINFO("ENTER MoleculeManipulation::invalidate()");
    if (!getSourceProcessor()) return;
    updateSelection();
    Processor::invalidate(inv);
    //LINFO("ENTER MoleculeManipulation::invalidate()");
}

// private methods
//
void MoleculeManipulation::updateSelection() {
    //LINFO("Calling MoleculeManipulation::updateSelection()");
    const MoleculeCollection* collection = getInputMoleculeCollection();
    if (!collection) {
        LERROR("Collection is NULL at MoleculeManipulation::updateSelection()");
        return;
    }
    //LINFO("Setting collection at MoleculeManipulation::updateSelection()");
    moleculeSelection_.setInputMolecules(collection);
    //LINFO("REturning from MoleculeManipulation::updateSelection()");
}

} // namespace
