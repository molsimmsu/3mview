#ifndef VRN_TRANSFERFUNCTIONEDITOR_H
#define VRN_TRANSFERFUNCTIONEDITOR_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/transfuncproperty.h"
#include "voreen/core/properties/intproperty.h"
using namespace voreen;

class TransferFunctionEditor : public Processor {
public:
	TransferFunctionEditor();
	virtual ~TransferFunctionEditor() {}
    virtual Processor* create() const { return new TransferFunctionEditor(); }

    virtual std::string getClassName() const { return "TransferFunctionEditor";   }
    virtual std::string getCategory() const  { return "Input";            }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE;  }
    
    virtual void process() {}

protected:
    virtual void setDescriptions() {
        setDescription("Transfer function editor");
    }
    
private:
	TransFuncProperty func1_;
	TransFuncProperty func2_;
	TransFuncProperty func3_;
	TransFuncProperty func4_;
	IntProperty interactionCoarseness_; 

};

#endif
