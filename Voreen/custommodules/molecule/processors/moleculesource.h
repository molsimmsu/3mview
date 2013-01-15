#ifndef VRN_MOLECULESOURCE_H
#define VRN_MOLECULESOURCE_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "../ports/moleculeport.h"

using namespace voreen;

class MoleculeSource : public Processor {
public:
    MoleculeSource();

    // virtual constructor
    virtual Processor* create() const { return new MoleculeSource(); }

    // documentary functions
    virtual std::string getClassName() const { return "MoleculeSource";       }
    virtual std::string getCategory() const  { return "Input";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }

protected:
    virtual void setDescriptions() {
        setDescription("Load molecules from files and generate test molecules");
    }

    virtual void process() {}

private:
    /**
     * Reads molecule from file and assigns the returned molecule to the outport.
     */
    void readMolecule();
    
    Molecule* loadMoleculeFromFile(const std::string& filename)
        throw (VoreenException);

    // ports and properties
    MoleculePort outport_;
    
    FileDialogProperty inputFile_;
    ButtonProperty loadMolecule_;
    ButtonProperty clearMolecule_;
    
};

#endif // VRN_MOLECULESOURCE_H
