#ifndef VRN_STEREOMODULE_H
#define VRN_STEREOMODULE_H

#include "voreen/core/voreenmodule.h"
using namespace voreen;

class StereoModule : public VoreenModule {

public:
    StereoModule(const std::string& modulePath);

    virtual std::string getDescription() const { 
        return "Module for stereo 3D visualization"; 
    }
};

#endif // VRN_STEREOMODULE_H
