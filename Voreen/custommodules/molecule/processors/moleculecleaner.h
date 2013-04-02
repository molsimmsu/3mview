#ifndef VRN_MoleculeCleaner_H
#define VRN_MoleculeCleaner_H

#include "moleculecoprocessor.h"
#include "../properties/moleculeurllistproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/optionproperty.h"
using namespace voreen;

class MoleculeCleaner : public MoleculeCoProcessor {

public:
    MoleculeCleaner();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "MoleculeCleaner";  }
    virtual std::string getCategory() const  { return "Input";           }
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; }
    
    virtual void updateSelection();

protected:
    virtual void setDescriptions() {
        setDescription("Removes selected types of residues from the molecules");
    }

    virtual void process() {}

    static const std::string loggerCat_;

private:
    void UpdateResSelection();
    void removeHydrogens();
    
    StringOptionProperty    resType_;
    BoolProperty invertSelection_;
    ButtonProperty removeHydrogens_;
    BoolProperty createNew_;
    ButtonProperty clearButton_;
    MoleculeURLListProperty moleculeURLlist_;

};

#endif
