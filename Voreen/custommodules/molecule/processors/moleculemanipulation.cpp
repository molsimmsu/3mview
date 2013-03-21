#include "moleculemanipulation.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"

namespace voreen {

MoleculeManipulation::MoleculeManipulation()
    : ManipulationBase()
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())
{
    //LINFO("ENTER MoleculeManipulation::MoleculeManipulation()");
	addProperty(moleculeURLlist_);
	//LINFO("EXIT MoleculeManipulation::MoleculeManipulation()");
}

MoleculeManipulation::~MoleculeManipulation() {
    
}

Processor* MoleculeManipulation::create() const {
    //std::cout << "MoleculeManipulation::create()" << std::endl;
    return new MoleculeManipulation();
}

void MoleculeManipulation::updateSelection() {
    //LWARNING("MoleculeManipulation::updateSelection()");
    MoleculeCoProcessor::updateSelection();
    const MoleculeCollection* collection = getInputMoleculeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    moleculeURLlist_.clear();
    for (size_t i = 0; i < collection->size(); i++) {
        moleculeURLlist_.addMolecule(collection->at(i));
        //LWARNING("MoleculeManipulation::updateSelection() added Molecule");
    }
}

void MoleculeManipulation::applyTransformation(tgt::vec3 offset, tgt::mat4 matrix) {
    //LINFO("Enter MoleculeManipulation::applyTransformation()");
    ManipulationBase::applyTransformation(offset, matrix);
    
    //moleculeSelection_.setAllSelected(true); // TODO Fix Molecule selection and remove
    
        const MoleculeCollection* collection = moleculeURLlist_.getMolecules(true);
        if (collection == 0 || collection->size() == 0) {
            //LINFO("Exit MoleculeManipulation::applyTransformation() at return");
            return;
        }
        
        for (size_t i = 0; i < collection->size(); i++) {
            Molecule* molecule = collection->at(i);
            if (!molecule) {
                //LWARNING("Molecule is 0 at MoleculeManipulation::applyTransformation()");
                continue;
            }

            tgt::mat4 offsetMatrix(
                1, 0, 0, offset[0],
                0, 1, 0, offset[1],
                0, 0, 1, offset[2],
                0, 0, 0, 1
            );
            
            molecule->transform(matrix * offsetMatrix);
        }
        
        Processor* processor = getSourceProcessor();
        if (processor != 0)
            processor->getPort("moleculecollection")->invalidatePort();
            
     //LINFO("Exit MoleculeManipulation::applyTransformation() at the end");
}

} // namespace
