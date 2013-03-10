#ifndef VRN_DOMAINDATABASEADMINISTRATION_H
#define VRN_DOMAINDATABASEADMINISTRATION_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/callmemberaction.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/filedialogproperty.h"
using namespace voreen;

#include "tgt/filesystem.h"

#include <string>

class DomainDatabaseAdministration : public Processor {
public:
    DomainDatabaseAdministration();
    
    virtual std::string getClassName() const { return "DomainDatabaseAdministration"; }
    virtual std::string getCategory() const  { return "Database"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual Processor* create() const { return new DomainDatabaseAdministration(); }
    
    virtual void process() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Add and remove domains, make BLAST database, get statistics");
    }
    
    void addDomain();
    
private:
    FileDialogProperty selectDomain_;
    ButtonProperty addDomain_;
    
    static std::string loggerCat_;
};

#endif
