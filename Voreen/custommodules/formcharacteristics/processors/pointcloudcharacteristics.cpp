#include "pointcloudcharacteristics.h"

MoleculeGeometryBuilder::MoleculeGeometryBuilder()
  : Processor()
  , inport_(Port::INPORT, "pointCloud", "Weighted Point Cloud Input")
  // TODO
  // , outport_(Port::OUTPORT, "geometry", "Geometry Output")
{
    addPort(inport_);
    // TODO
    //addPort(outport_);
}

void MoleculeGeometryBuilder::process() {

}
