#ifndef VRN_ElectronDensityModule_QT_H
#define VRN_ElectronDensityModule_QT_H

#include "voreen/qt/voreenmoduleqt.h"

namespace voreen {

class ElectronDensityModuleQt : public VoreenModuleQt {

public:
    ElectronDensityModuleQt(const std::string& modulePath);

protected:
    static const std::string loggerCat_;
};

} // namespace

#endif // VRN_ElectronDensityModule_QT_H

