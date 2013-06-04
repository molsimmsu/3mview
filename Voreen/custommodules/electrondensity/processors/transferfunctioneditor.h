#ifndef VRN_TRANSFERFUNCTIONEDITOR_H
#define VRN_TRANSFERFUNCTIONEDITOR_H

#include "densitymapcoprocessor.h"
#include "../datastructures/moleculevolume.h"
#include "voreen/core/properties/transfuncproperty.h"
#include "voreen/core/processors/processorwidget.h"
#include "../properties/transfunclistproperty.h"
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
    
    virtual void invalidate(int inv = INVALID_RESULT);

protected:
    virtual void setDescriptions() {
        setDescription("Transfer function editor");
    }
    
    static const std::string loggerCat_;
    
private:
    TransFuncListProperty TFList_;

};

#endif
