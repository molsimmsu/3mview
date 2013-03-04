#ifndef VRN_MOLECULESELECTOR_H
#define VRN_MOLECULESELECTOR_H

#include "voreen/core/processors/processor.h"
#include "../ports/moleculeport.h"
#include "../ports/moleculecollectionport.h"
#include "voreen/core/properties/intproperty.h"

namespace voreen {

class Molecule;
class ProcessorWidgetFactory;

/**
 * Selects a single molecule out of a input collection.
 */
class MoleculeSelector : public Processor {

public:
    MoleculeSelector();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "MoleculeSelector";  }
    virtual std::string getCategory() const  { return "Input";           }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE; }
    virtual bool isUtility() const           { return true; }

    virtual void invalidate(int inv = INVALID_RESULT);

protected:
    virtual void setDescriptions() {
        setDescription("Selects a single molecule from the input collection.");
    }

    virtual void process();
    virtual void initialize() throw (tgt::Exception);

    IntProperty moleculeID_;

    /// Inport for the molecule collection.
    MoleculeCollectionPort inport_;

    /// The molecule port the selected molecule is written to.
    MoleculePort outport_;

    static const std::string loggerCat_;

private:
    void adjustToMoleculeCollection();

};

} // namespace

#endif
