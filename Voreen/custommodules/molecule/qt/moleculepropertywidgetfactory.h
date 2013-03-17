#ifndef VRN_MOLECULEPROPERTYWIDGETFACTORY_H
#define VRN_MOLECULEPROPERTYWIDGETFACTORY_H

#include "voreen/core/properties/propertywidgetfactory.h"

namespace voreen {

class MoleculePropertyWidgetFactory : public PropertyWidgetFactory {
public:
    virtual PropertyWidget* createWidget(Property*) const;

    virtual bool lazyInstantiation(Property* prop) const;
};

} // namespace

#endif // VRN_MOLECULEPROPERTYWIDGETFACTORY_H