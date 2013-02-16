#include "weightedpointcloudport.h"

WeightedPointCloudPort::WeightedPointCloudPort(PortDirection direction, const std::string& name, const std::string& guiName,
             bool allowMultipleConnections,
             Processor::InvalidationLevel invalidationLevel)
: GenericPort<WeightedPointCloud>(direction, name, guiName, allowMultipleConnections, invalidationLevel)
{}
