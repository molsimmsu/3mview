#ifndef VRN_RunDock_H
#define VRN_RunDock_H

#include "moleculecoprocessor.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "../properties/moleculeurllistproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/properties/optionproperty.h"

#include "tgt/filesystem.h"

#include <string>

using namespace voreen;

class RunDock : public MoleculeCoProcessor {

public:
    RunDock();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "RunDock";  }
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
    void submit();
    void loadResult();
    void checkStatus();
    std::string molName1_;
    std::string molName2_;
       
    FileDialogProperty       workingDir_;
    MoleculeURLListProperty moleculeURLlist_;
    StringOptionProperty   runInterface_;
    IntProperty                  numMod_;
    StringOptionProperty   selectTarget_;
    StringOptionProperty       dockType_;
    StringProperty               status_;
    ButtonProperty         submitButton_;
    ButtonProperty           readButton_;
    ButtonProperty          checkButton_;
    
    

};

#endif
