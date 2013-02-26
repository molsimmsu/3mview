#ifndef VRN_GEOMETRYMODULE_QT_H
#define VRN_GEOMETRYMODULE_QT_H

#include "voreen/qt/voreenmoduleqt.h"

namespace voreen {

class GeometryModuleQt : public VoreenModuleQt {

public:
    GeometryModuleQt(const std::string& modulePath);

protected:
    static const std::string loggerCat_;
};

} // namespace

#endif // VRN_GEOMETRYMODULE_QT_H

