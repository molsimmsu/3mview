#ifndef VRN_MOLECULECOLLECTIONSOURCE_H
#define VRN_MOLECULECOLLECTIONSOURCE_H

#include "../ports/moleculecollectionport.h"
#include "../properties/moleculeurllistproperty.h"

#include "../../geometry/processors/manipulationbase.h"

#include "voreen/core/ports/coprocessorport.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/boolproperty.h"
using namespace voreen;

class MoleculeCollectionSource : virtual public Processor {
public:
    MoleculeCollectionSource();

    // virtual constructor
    virtual Processor* create() const { return new MoleculeCollectionSource(); }

    // documentary functions
    virtual std::string getClassName() const { return "MoleculeCollectionSource";       }
    virtual std::string getCategory() const  { return "Input";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void invalidate(int inv = INVALID_RESULT);
    
    void load(const std::string& path);
    void runTestFasta();
    MoleculeCollection* getMoleculeCollection();
    MoleculeCollection* getSelectedMoleculeCollection();

protected:
    virtual void setDescriptions() {
        setDescription("Load molecules from files and generate test molecules");
    }

    virtual void process() {}
    virtual void initialize() throw (tgt::Exception);
    
    static const std::string loggerCat_;

private:

    // ports and properties
    MoleculeCollectionPort outport_; ///< This outport must always contain valid data
    CoProcessorPort coProcessorPort_; ///< This outport must always contain valid data
    
    BoolProperty clearWater_;
    MoleculeURLListProperty moleculeURLlist_;
    
};

#endif // VRN_MOLECULECOLLECTIONSOURCE_H
