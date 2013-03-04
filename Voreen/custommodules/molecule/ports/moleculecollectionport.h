#ifndef VRN_MOLECULECOLLECTIONPORT_H
#define VRN_MOLECULECOLLECTIONPORT_H

#include "voreen/core/ports/genericport.h"
#include "../datastructures/moleculecollection.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API GenericPort<MoleculeCollection>;
#endif

class VRN_CORE_API MoleculeCollectionPort : public GenericPort<MoleculeCollection> {
public:
    MoleculeCollectionPort(PortDirection direction, const std::string& name, const std::string& guiName = "",
                 bool allowMultipleConnections = false,
                 Processor::InvalidationLevel invalidationLevel = Processor::INVALID_PROGRAM);
                 
    virtual std::string getClassName() const { return "MoleculeCollectionPort"; }
    
    virtual bool supportsCaching() const { return false; }

    virtual tgt::col3 getColorHint() const { return tgt::col3(0, 128, 255); }
};

} // namespace

#endif // VRN_MOLECULECOLLECTIONPORT_H
