#ifndef VRN_MANIPULATIONBASE_H
#define VRN_MANIPULATIONBASE_H

#include "voreen/core/ports/allports.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/cameraproperty.h"

class SpaceballEventListener;

namespace voreen {

class ManipulationBase : virtual public Processor {
public:
    ManipulationBase();
    virtual ~ManipulationBase();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "ManipulationBase";       }
    virtual std::string getCategory() const  { return "Manipulation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void applyTransformation(tgt::vec3 offset, tgt::mat4 matrix);
    
    tgt::mat4 GetRotMat();
    tgt::mat4 GetInvMat();
    
protected:
    virtual void setDescriptions() {
        setDescription("Base processor for manipulation");
    }

    virtual void process() {}
    

private:
    void guiEventHandler();
    
	StringOptionProperty manipulationType_;
	StringOptionProperty manipulationAxis_;
    BoolProperty invertDirection_;
	FloatProperty manipulationSlider_;
	CameraProperty camera_;
	
	//Timer* timer_;
	SpaceballEventListener* spaceballListener_;
	//EventHandler* eventHandler_;
};

} // namespace

#endif // VRN_MANIPULATIONBASE_H
