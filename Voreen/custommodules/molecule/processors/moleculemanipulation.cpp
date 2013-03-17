#include "moleculemanipulation.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"

namespace voreen {

MoleculeManipulation::MoleculeManipulation()
    : ManipulationBase()
    , MoleculeCoProcessor()
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

void MoleculeManipulation::updateSelection() {
  
}

void MoleculeManipulation::applyTransformation(tgt::vec3 offset, tgt::mat4 matrix) {
    //LINFO("Enter MoleculeManipulation::applyTransformation()");
    ManipulationBase::applyTransformation(offset, matrix);
    /*
    //moleculeSelection_.setAllSelected(true); // TODO Fix Molecule selection and remove
    
        const MoleculeCollection* collection = moleculeSelection_.getInputMolecules(); // TODO Fix to selected
        if (collection == 0 || collection->size() == 0) {
            LINFO("Exit MoleculeManipulation::applyTransformation() at return");
            return;
        }
        
        for (size_t i = 0; i < collection->size(); i++) {
            Molecule* molecule = collection->at(i);
            if (!molecule) {
                LWARNING("Molecule is 0 at MoleculeManipulation::applyTransformation()");
                continue;
            }
            LINFO("Start transform");
            tgt::mat4 transform = molecule->getTransformationMatrix();
            molecule->setTransformationMatrix(transform * matrix);
            LINFO("End transform");
        }
        
        Processor* processor = getSourceProcessor();
        if (processor != 0)
            processor->getPort("moleculecollection")->invalidatePort();
            */
     //LINFO("Exit MoleculeManipulation::applyTransformation() at the end");
}

void MoleculeManipulation::invalidate(int inv) {
    //LINFO("ENTER MoleculeManipulation::invalidate()");
    if (getSourceProcessor() == 0) return;
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
