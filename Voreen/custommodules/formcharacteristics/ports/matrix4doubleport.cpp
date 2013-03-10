#include "matrix4doubleport.h"

Matrix4doublePort::Matrix4doublePort(PortDirection direction, const std::string& name, const std::string& guiName,
             bool allowMultipleConnections,
             Processor::InvalidationLevel invalidationLevel)
: GenericPort<tgt::Matrix4<double> >(direction, name, guiName, allowMultipleConnections, invalidationLevel)
{}
