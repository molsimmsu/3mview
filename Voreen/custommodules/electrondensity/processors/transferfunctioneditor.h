#ifndef VRN_TRANSFERFUNCTIONEDITOR_H
#define VRN_TRANSFERFUNCTIONEDITOR_H

#include "densitymapcoprocessor.h"
#include "../datastructures/moleculevolume.h"
#include "voreen/core/properties/transfuncproperty.h"
using namespace voreen;

#include <vector>
#include <sstream>

class TransferFunctionEditor : public DensityMapCoProcessor {
public:
	TransferFunctionEditor();
	virtual ~TransferFunctionEditor() {}
    virtual Processor* create() const { return new TransferFunctionEditor(); }

    virtual std::string getClassName() const { return "TransferFunctionEditor";   }
    virtual std::string getCategory() const  { return "Visualize";            }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL;  }
    
    virtual void process() {}
    
    virtual void updateSelection();

protected:
    virtual void setDescriptions() {
        setDescription("Transfer function editor");
    }
    
private:
	std::vector<TransFuncProperty> tf_;

};

#endif
