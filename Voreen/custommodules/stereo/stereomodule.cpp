#include "stereomodule.h"

#include "processors/stereomeshentryexitpoints.h"
using namespace voreen;

StereoModule::StereoModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Stereo");
    
    registerProcessor(new StereoMeshEntryExitPoints());
}


