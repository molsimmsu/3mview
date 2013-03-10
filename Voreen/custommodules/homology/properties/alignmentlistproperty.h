#ifndef VRN_AlignmentListProperty_H
#define VRN_AlignmentListProperty_H

#include "voreen/core/properties/templateproperty.h"

#include "../datastructures/alignmentlist.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API TemplateProperty<AlignmentList>;
#endif

class VRN_CORE_API AlignmentListProperty : public TemplateProperty<AlignmentList> {
public:
    AlignmentListProperty(const std::string& id, const std::string& guiText, const AlignmentList& value,
        int invalidationLevel=Processor::INVALID_RESULT);
    AlignmentListProperty();
    virtual ~AlignmentListProperty() {}

    virtual Property* create() const;

    virtual std::string getClassName() const       { return "AlignmentListProperty"; }
    virtual std::string getTypeDescription() const { return "AlignmentList"; }
};

}

#endif
