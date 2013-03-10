#ifndef VRN_MultipleSequenceFinder_H
#define VRN_MultipleSequenceFinder_H

#include "../../molecule/processors/moleculecoprocessor.h"
#include "../properties/alignmentlistproperty.h"

#include "voreen/core/properties/callmemberaction.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/properties/intproperty.h"
using namespace voreen;

#include "tgt/filesystem.h"

#include <string>

class MultipleSequenceFinder : public MoleculeCoProcessor {
public:
    MultipleSequenceFinder();
    
    virtual std::string getClassName() const { return "MultipleSequenceFinder"; }
    virtual std::string getCategory() const  { return "Database"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual Processor* create() const { return new MultipleSequenceFinder(); }
    
    virtual void process() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Find homologous domains from BLAST database for each sequence from a given collection");
    }
    
    /*
     * Find domains matching a given sequence and put them
     * into the AlignmentListProperty
     */
    void findDomains();
    
    /*
     * Read domains stored in the AlignmentListProperty
     * and load a given number of them (stored in maxDomainsToLoad_ property)
     * into the molecule collection
     */
    void loadDomains();
    
private:
    FileDialogProperty loadSequence_;
    StringProperty sequenceText_;
    ButtonProperty findDomains_;
    AlignmentListProperty alignmentList_;
    IntProperty maxDomainsToLoad_;
    
    static std::string loggerCat_;
};

#endif
