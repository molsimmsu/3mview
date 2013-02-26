#include "electrondensitymanipulation.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"

namespace voreen {


ElectronDensityManipulation::ElectronDensityManipulation()
    : ManipulationBase()
    , volumeSelection_("volumeSelection", "Volume selection")
{
    LINFO("ENTER ElectronDensityManipulation::ElectronDensityManipulation()");
	addProperty(volumeSelection_);
	LINFO("EXIT ElectronDensityManipulation::ElectronDensityManipulation()");
}

ElectronDensityManipulation::~ElectronDensityManipulation() {
    
}

Processor* ElectronDensityManipulation::create() const {
    std::cout << "ElectronDensityManipulation::create()" << std::endl;
    return new ElectronDensityManipulation();
}

void ElectronDensityManipulation::applyTransformation(tgt::vec3 offset, tgt::mat4 matrix) {
    std::cout << "ElectronDensityManipulation::applyTransformation()" << std::endl;
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

void ElectronDensityManipulation::invalidate(int inv) {
    LINFO("ENTER ElectronDensityManipulation::invalidate()");
    if (!getSourceProcessor()) return;
    updateSelection();
    Processor::invalidate(inv);
    LINFO("ENTER ElectronDensityManipulation::invalidate()");
}

// private methods
//
DensityMapCollectionSource* ElectronDensityManipulation::getSourceProcessor() const {
    std::vector<Processor*> processors = inport_.getConnectedProcessors();
    if (processors.size() == 0) {
        LWARNING("Processors.size() is 0 at ElectronDensityManipulation::getSourceProcessor()");
        return 0;
    }
    
    Processor* processor = processors[0];
    
    if (processor == 0) {
        LWARNING("Processor is NULL at ElectronDensityManipulation::getSourceProcessor()");
        return 0;
    }
    
    if (typeid(*processor) != typeid(DensityMapCollectionSource)) {
        LWARNING("Processor is not a DensityMapCollectionSource at ElectronDensityManipulation::getSourceProcessor()");
        return 0;
    }
    
    return static_cast<DensityMapCollectionSource*>(processor);
}

const VolumeCollection* ElectronDensityManipulation::getInputVolumeCollection() const {
    DensityMapCollectionSource* source = getSourceProcessor();
    if (source == 0) {
        LWARNING("Source is NULL at ElectronDensityManipulation::getInputVolumeCollection()");
        return 0;
    }
    
    return source->getSelectedVolumeCollection();
}

void ElectronDensityManipulation::updateSelection() {
    LINFO("Calling ElectronDensityManipulation::updateSelection()");
    const VolumeCollection* collection = getInputVolumeCollection();
    if (!collection) {
        LERROR("Collection is NULL at ElectronDensityManipulation::updateSelection()");
        return;
    }
    LINFO("Setting collection at ElectronDensityManipulation::updateSelection()");
    volumeSelection_.setInputVolumes(collection);
    LINFO("REturning from ElectronDensityManipulation::updateSelection()");
}

/*
void ElectronDensityManipulation::forceUpdate() {

        const VolumeCollection* collection = volumeSelection_.getSelectedVolumes();
        if (collection == 0 || collection->size() == 0) return;
        
        for (size_t i = 0; i < collection->size(); i++) {
            VolumeBase* volume = collection->at(i);
            if (typeid(*volume) != typeid(Volume)) {
                LWARNING("Base class is not an instance of Volume");
                continue;
            }
            
            if (manipulationType_.get() == "move") {
                tgt::vec3 offset =  volume->getOffset();
                
                float offsetSize = 10;
                if (invertDirection_.get() == true) offsetSize = -offsetSize;
                 if (manipulationAxis_.get() == "x") offset[0] += offsetSize;
                else if (manipulationAxis_.get() == "y") offset[1] += offsetSize;
                else if (manipulationAxis_.get() == "z") offset[2] += offsetSize;
                
                static_cast<Volume*>(volume)->setOffset(offset);
            }
            else if (manipulationType_.get() == "rotate") {
                tgt::mat4 transform = volume->getPhysicalToWorldMatrix();
                tgt::mat4 transformMatrix;
                
                float angle = 10.f * 3.141592f / 180.f;
                if (invertDirection_.get() == true) angle = - angle;
                float sina = sin(angle);
                float cosa = cos(angle);
                 if (manipulationAxis_.get() == "x")
                transformMatrix = tgt::mat4(
                    1.0f, 0.0f, 0.0f, 0.0f, 
                    0.0f, cosa,-sina, 0.0f, 
                    0.0f, sina, cosa, 0.0f, 
                    0.0f, 0.0f, 0.0f, 1.0f
                );
                else if (manipulationAxis_.get() == "y")
                transformMatrix = tgt::mat4(
                    cosa, 0.0f,-sina, 0.0f, 
                    0.0f, 1.0f, 0.0f, 0.0f, 
                    sina, 0.0f, cosa, 0.0f, 
                    0.0f, 0.0f, 0.0f, 1.0f
                );
                else if (manipulationAxis_.get() == "z")
                transformMatrix = tgt::mat4(
                    cosa,-sina, 0.0f, 0.0f, 
                    sina, cosa, 0.0f, 0.0f, 
                    0.0f, 0.0f, 1.0f, 0.0f, 
                    0.0f, 0.0f, 0.0f, 1.0f
                );
                
                static_cast<Volume*>(volume)->setPhysicalToWorldMatrix(transform * transformMatrix);
            }
        }
        
        Processor* processor = getSourceProcessor();
        if (processor != 0)
            processor->getPort("volumecollection")->invalidatePort();
    
}
*/
} // namespace
