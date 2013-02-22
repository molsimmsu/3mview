#include "electrondensitymoduleqt.h"

#include "qt/electrondensitypropertywidgetfactory.h"

namespace voreen {

const std::string ElectronDensityModuleQt::loggerCat_("voreen.opencl.ElectronDensityModuleQt");

ElectronDensityModuleQt::ElectronDensityModuleQt(const std::string& modulePath)
    : VoreenModuleQt(modulePath)
{
    setName("Electron Density (Qt)");

    registerPropertyWidgetFactory(new ElectronDensityPropertyWidgetFactory());
}

} // namespace

