#include "collectionboundingboxesbuilder.h"

#include "tgt/vector.h"
#include "tgt/matrix.h"

const std::string CollectionBoundingBoxesBuilder::loggerCat_("voreen.base.CollectionBoundingBoxesBuilder");

CollectionBoundingBoxesBuilder::CollectionBoundingBoxesBuilder()
    : volumeInport_(Port::INPORT, "volumeInport", "Volume Input")
    , geometryInport_(Port::INPORT, "geometryInport", "Geometry Input")
    , outport_(Port::OUTPORT, "outport", "Geometry Output")
{
    addPort(volumeInport_);
    addPort(geometryInport_);
    addPort(outport_);
    
    outport_.setData(new MeshListGeometry(), true);
}

Processor* CollectionBoundingBoxesBuilder::create() const {
    return new CollectionBoundingBoxesBuilder();
}

bool CollectionBoundingBoxesBuilder::isReady() const {
    if (!isInitialized())
        return false;

    return (volumeInport_.isReady() || geometryInport_.isReady());
}

MeshListGeometry* CollectionBoundingBoxesBuilder::getGeometry() {
    return static_cast<MeshListGeometry*>(outport_.getWritableData());
}

void CollectionBoundingBoxesBuilder::process() {
    if (!isReady()) return;
    getGeometry()->clear();
    tgt::vec3 geomLlf, geomUrb;
    const VolumeCollection* volumeCollection = volumeInport_.getData();
    if (volumeCollection) {
        for (size_t i = 0; i < volumeCollection->size(); i++) {
            const VolumeBase* volume = volumeCollection->at(i);
            geomLlf = volume->getLLF();
            geomUrb = volume->getURB();

            MeshGeometry bb = MeshGeometry::createCube(geomLlf, geomUrb);
            bb.transform(volume->getPhysicalToWorldMatrix());
            
            getGeometry()->addMesh(bb);
        }
    }
    else if (geometryInport_.hasData()) {
        /*tgt::Bounds bounds = geometryInport_.getData()->getBoundingBox();
        geomLlf = bounds.getLLF();
        geomUrb = bounds.getURB();
        
        getGeometry()->addMesh(MeshGeometry::createCube(geomLlf, geomUrb));*/
    }
    else {
        tgtAssert(false, "no input has data"); //< should never get here
    }
}
