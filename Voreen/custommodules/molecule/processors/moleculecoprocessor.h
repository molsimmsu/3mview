#ifndef VRN_MoleculeCOPROCESSOR_H
#define VRN_MoleculeCOPROCESSOR_H

#include "moleculecollectionsource.h"
#include "voreen/core/ports/allports.h"

namespace voreen {
/*
 * Base co-processor for handling density maps
 */
class MoleculeCoProcessor : virtual public Processor {
public:
    MoleculeCoProcessor();
    virtual ~MoleculeCoProcessor() {}
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "MoleculeCoProcessor";       }
    virtual std::string getCategory() const  { return "Molecule Co-Processor";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void process() {}
    
    /*
     * Copy and paste this function in your derived classes to update selection
     */
    virtual void updateSelection() { }
    
protected:
    virtual void setDescriptions() {
        setDescription("Base co-processor for molecules operating");
    }

	const MoleculeCollection* getInputMoleculeCollection() const;
	MoleculeCollectionSource* getSourceProcessor() const;

private:
	CoProcessorPort inport_;
};

} // namespace

#endif // VRN_MoleculeCOPROCESSOR_H
