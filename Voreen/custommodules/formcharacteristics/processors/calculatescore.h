#ifndef VRN_CALCULATESCORE_H
#define VRN_CALCULATESCORE_H

#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/processors/processor.h"
using namespace voreen;

class CalculateScore : public Processor {
public:
    CalculateScore();

    // virtual constructor
    virtual Processor* create() const { return new CalculateScore(); }

    // documentary functions
    virtual std::string getClassName() const { return "CalculateScore";         }
    virtual std::string getCategory()  const { return "Form Analysis";         }
    virtual CodeState getCodeState()   const { return CODE_STATE_EXPERIMENTAL; }



protected:
    virtual void setDescriptions() {
        setDescription("Accepts an Electron density map as input, outputs number - score (less = better)");
    }

    virtual void process();

private:
    VolumePort  		  volinport_;
    StringProperty         output_;  
    
    const std::string loggerCat_;
 
};

#endif // VRN_CALCULATESCORE_H
