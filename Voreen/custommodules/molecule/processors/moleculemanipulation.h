#ifndef VRN_MOLECULEMANIPULATION_H
#define VRN_MOLECULEMANIPULATION_H

#include "moleculecollectionsource.h"
#include "moleculecoprocessor.h"

#include "../../geometry/processors/manipulationbase.h"
#include "../properties/moleculeselectionproperty.h"

namespace voreen {
 
class MoleculeManipulation : virtual public ManipulationBase, virtual public MoleculeCoProcessor {
public:
    MoleculeManipulation();
    virtual ~MoleculeManipulation();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "MoleculeManipulation";       }
    virtual std::string getCategory() const  { return "Molecule Manipulation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void invalidate(int inv = INVALID_RESULT);
    
    void process() {}
    
    void updateSelection();
    
protected:
    virtual void setDescriptions() {
        setDescription("Processor for molecule manipulation");
    }

private:
    void updateSelection();
    
    virtual void applyTransformation(tgt::vec3 offset, tgt::mat4 matrix);
    
    MoleculeSelectionProperty moleculeSelection_;
};

} // namespace

#endif // VRN_MOLECULEMANIPULATION_H
