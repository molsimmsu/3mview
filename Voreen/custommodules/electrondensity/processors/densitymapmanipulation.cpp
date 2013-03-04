#include "densitymapmanipulation.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"

namespace voreen {

DensityMapManipulation::DensityMapManipulation()
    : ManipulationBase()
    , volumeSelection_("volumeSelection", "Volume selection")
{
    LINFO("ENTER DensityMapManipulation::DensityMapManipulation()");
	addProperty(volumeSelection_);
	LINFO("EXIT DensityMapManipulation::DensityMapManipulation()");
}

DensityMapManipulation::~DensityMapManipulation() {
    
}

Processor* DensityMapManipulation::create() const {
    std::cout << "DensityMapManipulation::create()" << std::endl;
    return new DensityMapManipulation();
}

void DensityMapManipulation::applyTransformation(tgt::vec3 offset, tgt::mat4 matrix) {
    std::cout << "DensityMapManipulation::applyTransformation()" << std::endl;
    ManipulationBase::applyTransformation(offset, matrix);
    
        const VolumeCollection* collection = volumeSelection_.getSelectedVolumes();
        if (collection == 0 || collection->size() == 0) return;
        
        for (size_t i = 0; i < collection->size(); i++) {
            VolumeBase* volume = collection->at(i);
            
            if (typeid(*volume) != typeid(Volume)) {
                LWARNING("Base class is not an instance of Volume");
                continue;
            }
            
            tgt::vec3 volumeOffset = volume->getOffset() + offset;
            static_cast<Volume*>(volume)->setOffset(volumeOffset);
            
            tgt::mat4 transform = volume->getPhysicalToWorldMatrix();
            static_cast<Volume*>(volume)->setPhysicalToWorldMatrix(transform * matrix);
        }
        
        Processor* processor = getSourceProcessor();
        if (processor != 0)
            processor->getPort("volumecollection")->invalidatePort();
}

void DensityMapManipulation::invalidate(int inv) {
    LINFO("ENTER DensityMapManipulation::invalidate()");
    if (!getSourceProcessor()) return;
    updateSelection();
    Processor::invalidate(inv);
    LINFO("ENTER DensityMapManipulation::invalidate()");
}

// private methods
//
void DensityMapManipulation::updateSelection() {
    LINFO("Calling DensityMapManipulation::updateSelection()");
    const VolumeCollection* collection = getInputVolumeCollection();
    if (!collection) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    LINFO("Setting collection at DensityMapManipulation::updateSelection()");
    volumeSelection_.setInputVolumes(collection);
    LINFO("REturning from DensityMapManipulation::updateSelection()");
}

} // namespace
