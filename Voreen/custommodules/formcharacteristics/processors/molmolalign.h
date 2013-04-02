#ifndef VRN_MOLMOLALIGN_H
#define VRN_MOLMOLALIGN_H

#include "../../molecule/ports/moleculeport.h"
#include "../../molecule/processors/moleculecoprocessor.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/processors/processor.h"
#include "../datastructures/pointcloud.h"

using namespace voreen;

class MolMolAlign : public MoleculeCoProcessor {
public:
    MolMolAlign();

    // virtual constructor
    virtual Processor* create() const { return new MolMolAlign(); }

    // documentary functions
    virtual std::string getClassName() const { return "MolMolAlign";        }
    virtual std::string getCategory()  const { return "Form Analysis";         }
    virtual CodeState getCodeState()   const { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void updateSelection();

protected:
    virtual void setDescriptions() {
        setDescription("Accepts two molecules as input, fits one of them to another");
    }

    virtual void process() {}
    
    void align();

private:

    MoleculeURLListProperty moleculeURLlist_;
    StringOptionProperty   tobealigned_;
    BoolProperty         createNew_;  
    ButtonProperty         align_;  

    tgt::mat4 GetAlignment(const Molecule* vol);

    static const std::string loggerCat_; ///< category used in logging
};

#endif // VRN_VOLVOLALIGN_H
