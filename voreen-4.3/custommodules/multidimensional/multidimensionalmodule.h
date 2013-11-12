#ifndef VRN_MULTIDIMENSIONALMODULE_H
#define VRN_MULTIDIMENSIONALMODULE_H

#include "voreen/core/voreenmodule.h"

namespace voreen {

class MultidimensionalModule: public VoreenModule {

public:
    MultidimensionalModule(const std::string& modulePath);

    virtual std::string getDescription() const {
        return "This module contains tools for multidimensional rendering. ";
    }
};

} // namespace

#endif //VRN_MULTIDIMENSIONALMODULE_H
