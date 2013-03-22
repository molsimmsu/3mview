#ifndef VRN_MOLMOLALIGN_H
#define VRN_MOLMOLALIGN_H

#include "../../molecule/ports/moleculeport.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/processors/processor.h"
#include "../datastructures/pointcloud.h"

using namespace voreen;

class MolMolAlign : public Processor {
public:
    MolMolAlign();

    // virtual constructor
    virtual Processor* create() const { return new MolMolAlign(); }

    // documentary functions
    virtual std::string getClassName() const { return "MolMolAlign";        }
    virtual std::string getCategory()  const { return "Form Analysis";         }
    virtual CodeState getCodeState()   const { return CODE_STATE_EXPERIMENTAL; }

protected:
    virtual void setDescriptions() {
        setDescription("Accepts two molecules as input, fits one of them to another");
    }

    virtual void process() {}
    
    void align();

private:

    MoleculePort           molinport1_;
    MoleculePort           molinport2_;
    MoleculePort           outport_;
    StringOptionProperty   tobealigned_;
    ButtonProperty         align_;  


    tgt::mat4 GetAlignment(const Molecule* vol);

    static const std::string loggerCat_; ///< category used in logging
};

#endif // VRN_VOLVOLALIGN_H
