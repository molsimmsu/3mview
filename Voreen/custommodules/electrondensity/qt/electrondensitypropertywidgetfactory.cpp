#include "electrondensitypropertywidgetfactory.h"

#include "../properties/volumeselectionproperty.h"
#include "volumeselectionpropertywidget.h"

namespace voreen {

PropertyWidget* ElectronDensityPropertyWidgetFactory::createWidget(Property* prop) const {

    if (dynamic_cast<VolumeSelectionProperty*>(prop))
        return new VolumeSelectionPropertyWidget(static_cast<VolumeSelectionProperty*>(prop), 0);

    return 0;
}

bool ElectronDensityPropertyWidgetFactory::lazyInstantiation(Property* prop) const {

    if (dynamic_cast<VolumeSelectionProperty*>(prop))
        return false;

    return true;
}

} // namespace voreen

