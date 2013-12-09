#include "tableport.h"
using namespace voreen;

TablePort::TablePort(PortDirection direction, const std::string& name, const std::string& guiName,
             bool allowMultipleConnections,
             Processor::InvalidationLevel invalidationLevel)
: GenericPort<Molecule>(direction, name, guiName, allowMultipleConnections, invalidationLevel)
{}
