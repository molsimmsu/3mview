#include "tablemodule.h"

#include "processors/neighbourfinder.h"

#include <iostream>

namespace voreen {

TableModule::TableModule(const std::string& modulePath)
    : VoreenModule(modulePath)
{
    setID("Table");
    setGuiName("Table");

    registerProcessor(new NeighbourFinder());
}

} // namespace
