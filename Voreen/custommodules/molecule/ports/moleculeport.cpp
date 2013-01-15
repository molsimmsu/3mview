#include "moleculeport.h"

MoleculePort::MoleculePort(PortDirection direction, const std::string& name, const std::string& guiName,
             bool allowMultipleConnections,
             Processor::InvalidationLevel invalidationLevel)
: GenericPort<Molecule>(direction, name, guiName, allowMultipleConnections, invalidationLevel)
{}
