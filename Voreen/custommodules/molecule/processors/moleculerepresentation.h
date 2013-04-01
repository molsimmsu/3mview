#ifndef VRN_MoleculeRepresentation_H
#define VRN_MoleculeRepresentation_H

#include "moleculecoprocessor.h"
#include "../properties/moleculeurllistproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/optionproperty.h"
using namespace voreen;

class MoleculeRepresentation : public MoleculeCoProcessor {

public:
    MoleculeRepresentation();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "MoleculeRepresentation";  }
    virtual std::string getCategory() const  { return "Input";           }
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; }
    
    virtual void updateSelection();

protected:
    virtual void setDescriptions() {
        setDescription("Select molecule representations");
    }

    virtual void process() {}

    static const std::string loggerCat_;

private:
    void updateRepresentation();
    
    StringOptionProperty    repType_;
    ButtonProperty selectButton_;
    MoleculeURLListProperty moleculeURLlist_;

};

#endif
