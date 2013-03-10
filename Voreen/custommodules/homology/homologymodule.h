#ifndef VRN_HOMOLOGYMODULE_H
#define VRN_HOMOLOGYMODULE_H

#include "voreen/core/voreenmodule.h"

namespace voreen {

class HomologyModule : public VoreenModule {

public:
    HomologyModule(const std::string& modulePath);

    virtual std::string getDescription() const { 
        return "Module for homology serach using BLAST"; 
    }
};

} // namespace

#endif // VRN_HOMOLOGYMODULE_H
