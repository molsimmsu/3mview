#include "alignmentlistproperty.h"
#include "voreen/core/properties/condition.h"

#include <sstream>

namespace voreen {

AlignmentListProperty::AlignmentListProperty(const std::string& id, const std::string& guiText,
                       const AlignmentList& value, int invalidationLevel)
    : TemplateProperty<AlignmentList>(id, guiText, value, invalidationLevel)
{}

AlignmentListProperty::AlignmentListProperty()
{}

Property* AlignmentListProperty::create() const {
    return new AlignmentListProperty();
}

}   // namespace
