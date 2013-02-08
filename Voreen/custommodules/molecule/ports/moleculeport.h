#ifndef VRN_MOLECULEPORT_H
#define VRN_MOLECULEPORT_H

#include "voreen/core/ports/genericport.h"
#include "../datastructures/molecule.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API GenericPort<Molecule>;
#endif

class VRN_CORE_API MoleculePort : public GenericPort<Molecule> {
public:
    MoleculePort(PortDirection direction, const std::string& name, const std::string& guiName = "",
                 bool allowMultipleConnections = false,
                 Processor::InvalidationLevel invalidationLevel = Processor::INVALID_PROGRAM);
                 
    virtual std::string getClassName() const { return "MoleculePort"; }
    
    virtual bool supportsCaching() const { return false; }

    virtual tgt::col3 getColorHint() const { return tgt::col3(0, 128, 255); }
};

} // namespace

#endif // VRN_MOLECULEPORT_H
