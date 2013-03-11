#ifndef VRN_COLLECTIONBOUNDINGBOXRENDERER_H
#define VRN_COLLECTIONBOUNDINGBOXRENDERER_H

#include "../ports/geometrycollectionport.h"
#include "voreen/core/ports/volumeport.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/ports/geometryport.h"
#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/properties/volumeurllistproperty.h"
using namespace voreen;

///Draws bounding box around the data set
class CollectionBoundingBoxesBuilder : public Processor {
public:
    CollectionBoundingBoxesBuilder();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "CollectionBoundingBoxesBuilder"; }
    virtual std::string getCategory() const  { return "Bounding Box"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE; }

    /// Returns true, if one of the inports is connected.
    virtual bool isReady() const;
    
    virtual void process();
    
    MeshListGeometry* getGeometry();

protected:
    virtual void setDescriptions() {
        setDescription("Draws bounding boxes around the input volumes and geometry. For volumes, the axis-aligned bounding box is computed in physical coordinates and then transformed into world-coordinates. For geometry, the axis-aligned bounding is computed directly in world coordinates.");
    }

private:
    VolumeCollectionPort volumeInport_;
    GeometryCollectionPort geometryInport_;
    GeometryPort outport_;

    static const std::string loggerCat_;
};

#endif
