#ifndef VRN_VolumeSelectionProperty_H
#define VRN_VolumeSelectionProperty_H

#include "voreen/core/properties/templateproperty.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API TemplateProperty<const VolumeCollection*>;
#endif

class VRN_CORE_API VolumeSelectionProperty : public TemplateProperty<const VolumeCollection*> {
public:
    VolumeSelectionProperty(const std::string& id, const std::string& guiText, const VolumeCollection* value,
        int invalidationLevel=Processor::INVALID_RESULT);
    VolumeSelectionProperty();
    virtual ~VolumeSelectionProperty() {}

    virtual Property* create() const;

    virtual std::string getClassName() const       { return "VolumeSelectionProperty"; }
    virtual std::string getTypeDescription() const { return "VolumeCollection"; }
};

}

#endif

