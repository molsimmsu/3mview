#ifndef VRN_SAMPLEPROPERTY_H
#define VRN_SAMPLEPROPERTY_H

#include "voreen/core/properties/templateproperty.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API TemplateProperty<SamplePropertyClass>;
#endif

class VRN_CORE_API SampleProperty : public TemplateProperty<SamplePropertyClass> {
public:
    SampleProperty(const std::string& id, const std::string& guiText,
                   const SamplePropertyClass& value,
                   Processor::InvalidationLevel invalidationLevel=Processor::INVALID_PROGRAM);
    SampleProperty();

    virtual Property* create() const;

    virtual std::string getClassName() const       { return "SampleProperty"; }
    virtual std::string getTypeDescription() const { return "Sample type description"; }

   

private:
    std::string getProgramAsString(std::string filename);

    std::string programDefines_;
    std::string originalProgramFilename_;

    cl::Program* program_;
};

}   // namespace

#endif
