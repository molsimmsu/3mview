#include "systemmodules.h"

#include "processors/qualitymodule.h"
#include "processors/adminmodule.h"
using namespace voreen;

SystemModules::SystemModules(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("System Modules");

    registerProcessor(new QualityModule());
    registerProcessor(new AdminModule());
}
