#ifndef VRN_MOLECULECOLLECTIONSOURCE_H
#define VRN_MOLECULECOLLECTIONSOURCE_H

#include "../ports/moleculecollectionport.h"

#include "voreen/core/ports/coprocessorport.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/buttonproperty.h"
using namespace voreen;

class MoleculeCollectionSource : public Processor {
public:
    MoleculeCollectionSource();

    // virtual constructor
    virtual Processor* create() const { return new MoleculeCollectionSource(); }

    // documentary functions
    virtual std::string getClassName() const { return "MoleculeCollectionSource";       }
    virtual std::string getCategory() const  { return "Input";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    MoleculeCollection* getMoleculeCollection();
    MoleculeCollection* getSelectedMoleculeCollection();

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
    MoleculeCollectionPort outport_; ///< This outport must always contain valid data
    CoProcessorPort coProcessorPort_; ///< This outport must always contain valid data
    
    FileDialogProperty inputFile_;
    ButtonProperty loadMolecule_;
    
};

#endif // VRN_MOLECULECOLLECTIONSOURCE_H
