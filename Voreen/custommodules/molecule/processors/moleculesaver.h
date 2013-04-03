#ifndef VRN_MoleculeSaver_H
#define VRN_MoleculeSaver_H

#include "moleculecoprocessor.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "../properties/moleculeurllistproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/stringproperty.h"

using namespace voreen;

class MoleculeSaver : public MoleculeCoProcessor {

public:
    MoleculeSaver();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "MoleculeSaver";  }
    virtual std::string getCategory() const  { return "Input";           }
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; }
    
    virtual void updateSelection();

protected:
    virtual void setDescriptions() {
        setDescription("Saves molecules to selected folder");
    }

    virtual void process() {}

    static const std::string loggerCat_;

private:
    void SaveMolecules();
    
    StringProperty   mergedName_;
    FileDialogProperty   folder_;
    BoolProperty mergeMolecules_;
    BoolProperty   removeHeader_;
    ButtonProperty   saveButton_;
    MoleculeURLListProperty moleculeURLlist_;
    

};

#endif
