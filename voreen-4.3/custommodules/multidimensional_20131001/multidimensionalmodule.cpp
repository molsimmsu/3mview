#include "multidimensionalmodule.h"

#include "processors/emptyvolumesource.h"
#include "processors/hypercubebuilder.h"

#include <iostream>

namespace voreen {

MultidimensionalModule::MultidimensionalModule(const std::string& modulePath)
    : VoreenModule(modulePath)
{
    setID("Multidimensional");
    setGuiName("Multidimensional");

    registerProcessor(new EmptyVolumeSource());
    registerProcessor(new HypercubeBuilder());
}

} // namespace
