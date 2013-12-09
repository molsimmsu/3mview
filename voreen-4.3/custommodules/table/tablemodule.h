#ifndef VRN_TABLEMODULE_H
#define VRN_TABLEMODULE_H

#include "voreen/core/voreenmodule.h"

namespace voreen {

class TableModule: public VoreenModule {

public:
    TableModule(const std::string& modulePath);

    virtual std::string getDescription() const {
        return "This module contains tools for table analysis. ";
    }
};

} // namespace

#endif //VRN_MULTIDIMENSIONALMODULE_H
