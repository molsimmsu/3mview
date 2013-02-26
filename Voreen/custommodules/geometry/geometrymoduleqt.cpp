#include "geometrymoduleqt.h"

namespace voreen {

const std::string GeometryModuleQt::loggerCat_("voreen.opencl.GeometryModuleQt");

GeometryModuleQt::GeometryModuleQt(const std::string& modulePath)
    : VoreenModuleQt(modulePath)
{
    setName("GeometryModuleQt (Qt)");
}

} // namespace

