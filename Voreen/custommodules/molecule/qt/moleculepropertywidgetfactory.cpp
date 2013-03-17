#include "moleculepropertywidgetfactory.h"
#include "moleculeurllistpropertywidget.h"
#include "../properties/moleculeurllistproperty.h"

namespace voreen {

PropertyWidget* MoleculePropertyWidgetFactory::createWidget(Property* prop) const {

    if (dynamic_cast<MoleculeURLListProperty*>(prop))
        return new MoleculeURLListPropertyWidget(static_cast<MoleculeURLListProperty*>(prop), 0);

    return 0;
}

bool MoleculePropertyWidgetFactory::lazyInstantiation(Property* prop) const {

    if (dynamic_cast<MoleculeURLListProperty*>(prop))
        return false;

    return true;
}

} // namespace voreen
