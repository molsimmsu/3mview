#ifndef VRN_TABLEPORT_H
#define VRN_TABLEPORT_H

#include "voreen/core/ports/genericport.h"
#include "../datastructures/table.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API GenericPort<TableFloat>;
#endif

class VRN_CORE_API TablePort : public GenericPort<TableFloat> {
public:
    TablePort(PortDirection direction, const std::string& name, const std::string& guiName = "",
                 bool allowMultipleConnections = false,
                 Processor::InvalidationLevel invalidationLevel = Processor::INVALID_PROGRAM);
                 
    virtual std::string getClassName() const { return "TablePort"; }
    
    virtual bool supportsCaching() const { return false; }

    virtual tgt::col3 getColorHint() const { return tgt::col3(222, 128, 111); }
};

} // namespace

#endif // VRN_MOLECULEPORT_H
