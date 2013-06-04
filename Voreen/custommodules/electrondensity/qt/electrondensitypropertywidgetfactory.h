#ifndef VRN_ELECTRONDENSITYPROPERTYWIDGETFACTORY_H
#define VRN_ELECTRONDENSITYPROPERTYWIDGETFACTORY_H

#include "voreen/core/properties/propertywidgetfactory.h"

namespace voreen {

class ElectronDensityPropertyWidgetFactory : public PropertyWidgetFactory {
public:
    virtual PropertyWidget* createWidget(Property*) const;

    virtual bool lazyInstantiation(Property* prop) const;
};

} // namespace

#endif // VRN_MOLECULEPROPERTYWIDGETFACTORY_H
