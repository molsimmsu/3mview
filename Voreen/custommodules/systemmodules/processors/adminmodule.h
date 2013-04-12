#ifndef VRN_AdminModule_H
#define VRN_AdminModule_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "tgt/filesystem.h"

using namespace voreen;

class AdminModule : public Processor {
public:
	AdminModule();
	virtual ~AdminModule() {}
    virtual Processor* create() const { return new AdminModule(); }

    virtual std::string getClassName() const { return "AdminModule";   }
    virtual std::string getCategory() const  { return "Input";            }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE;  }
    
    virtual void process() {}

protected:
    virtual void setDescriptions() {
        setDescription("Transfer function editor");
    }
    void killHex(){runcmd(0);}
    void runGanglia(){runcmd(1);}
    void runTop(){runcmd(2);}
    void runcmd(int num);
    
private:
	ButtonProperty  killHex_;
	ButtonProperty runGanglia_;
	ButtonProperty runTop_;

};
#endif
