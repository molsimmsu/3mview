#include "electrondensitymoduleqt.h"

#include "qt/electrondensitypropertywidgetfactory.h"

namespace voreen {

const std::string ElectronDensityModuleQt::loggerCat_("3MTK.ElectronDensityModuleQt");

ElectronDensityModuleQt::ElectronDensityModuleQt(const std::string& modulePath)
    : VoreenModuleQt(modulePath)
{
    setName("ElectronDensity (Qt)");

    registerPropertyWidgetFactory(new ElectronDensityPropertyWidgetFactory());
}

} // namespace
