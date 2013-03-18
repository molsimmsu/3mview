#include "mat4port.h"

mat4Port::mat4Port(PortDirection direction, const std::string& name, const std::string& guiName,
             bool allowMultipleConnections,
             Processor::InvalidationLevel invalidationLevel)
: GenericPort<tgt::mat4>(direction, name, guiName, allowMultipleConnections, invalidationLevel)
{}
