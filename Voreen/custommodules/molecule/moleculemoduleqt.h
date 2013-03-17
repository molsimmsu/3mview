#ifndef VRN_MOLECULEMODULE_QT_H
#define VRN_MOLECULEMODULE_QT_H

#include "voreen/qt/voreenmoduleqt.h"

namespace voreen {

class MoleculeModuleQt : public VoreenModuleQt {

public:
    MoleculeModuleQt(const std::string& modulePath);

protected:
    static const std::string loggerCat_;
};

} // namespace

#endif // VRN_MOLECULEMODULE_QT_H