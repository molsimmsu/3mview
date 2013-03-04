#include "moleculecollectionport.h"
using namespace voreen;

MoleculeCollectionPort::MoleculeCollectionPort(PortDirection direction, const std::string& name, const std::string& guiName,
             bool allowMultipleConnections,
             Processor::InvalidationLevel invalidationLevel)
: GenericPort<MoleculeCollection>(direction, name, guiName, allowMultipleConnections, invalidationLevel)
{}
