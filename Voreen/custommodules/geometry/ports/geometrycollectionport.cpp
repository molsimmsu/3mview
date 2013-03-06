#include "geometrycollectionport.h"
using namespace voreen;

GeometryCollectionPort::GeometryCollectionPort(PortDirection direction, const std::string& name, const std::string& guiName,
             bool allowMultipleConnections,
             Processor::InvalidationLevel invalidationLevel)
: GenericPort<GeometryCollection>(direction, name, guiName, allowMultipleConnections, invalidationLevel)
{}
