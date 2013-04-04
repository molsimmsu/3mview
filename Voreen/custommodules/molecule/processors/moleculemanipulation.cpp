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
    MoleculeCoProcessor::updateSelection();
    const MoleculeCollection* collection = getInputMoleculeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    moleculeURLlist_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        moleculeURLlist_.addMolecule(collection->at(i));
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
        
        tgt::ivec3 center(0.f, 0.f, 0.f);
        ///*** here ve calculete group rotation pivot
        for (size_t i = 0; i < collection->size(); i++) {
            Molecule* molecule = collection->at(i);
            tgt::ivec3 temp = molecule->getCenterOfMass();
            center += temp;
        }
        center = tgt::vec3(center[0] / float(collection->size()), center[1] / float(collection->size()), center[2] / float(collection->size()));
        
        ///*** aplying pivotcentric rotation
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
                        
            tgt::mat4 shiftMatrix(
                1, 0, 0, -center[0],
                0, 1, 0, -center[1],
                0, 0, 1, -center[2],
                0, 0, 0, 1
            );
            
            tgt::mat4 returnMatrix(
                1, 0, 0, center[0],
                0, 1, 0, center[1],
                0, 0, 1, center[2],
                0, 0, 0, 1
            );
            tgt::mat4 resultMatrix = returnMatrix * matrix * offsetMatrix * shiftMatrix;
            molecule->transform(resultMatrix);
            //molecule->transform(matrix.getRotationalPart());
            
        }
        
        Processor* processor = getSourceProcessor();
        if (processor != 0)
            processor->getPort("moleculecollection")->invalidatePort();
            
     //LINFO("Exit MoleculeManipulation::applyTransformation() at the end");
}

} // namespace
