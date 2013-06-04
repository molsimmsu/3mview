#include "electrondensitypropertywidgetfactory.h"
#include "transfunclistpropertywidget.h"
#include "../properties/transfunclistproperty.h"

namespace voreen {

PropertyWidget* ElectronDensityPropertyWidgetFactory::createWidget(Property* prop) const {

    if (dynamic_cast<TransFuncListProperty*>(prop))
        return new TransFuncListPropertyWidget(static_cast<TransFuncListProperty*>(prop), 0);

    return 0;
}

bool ElectronDensityPropertyWidgetFactory::lazyInstantiation(Property* prop) const {

    if (dynamic_cast<TransFuncListProperty*>(prop))
        return false;

    return true;
}

} // namespace voreen
