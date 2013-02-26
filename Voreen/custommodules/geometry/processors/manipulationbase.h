#ifndef VRN_MANIPULATIONBASE_H
#define VRN_MANIPULATIONBASE_H

#include "tgt/timer.h"
#include "tgt/event/eventhandler.h"
using namespace tgt;

#include "voreen/core/ports/allports.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/optionproperty.h"

namespace voreen {

class ManipulationBase;

class SpaceballEventListener : public EventListener {
public:
    SpaceballEventListener(ManipulationBase* manipulation);
    virtual ~SpaceballEventListener() {}

    virtual void timerEvent(TimeEvent* e);
    
private:
    ManipulationBase* manipulation_;
};

class ManipulationBase : public Processor {
public:
    ManipulationBase();
    virtual ~ManipulationBase();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "ManipulationBase";       }
    virtual std::string getCategory() const  { return "Manipulation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void applyTransformation(tgt::vec3 offset, tgt::mat4 matrix);
    
protected:
    CoProcessorPort inport_;

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
	
	Timer* timer_;
	SpaceballEventListener* spaceballListener_;
	EventHandler* eventHandler_;
};

} // namespace

#endif // VRN_MANIPULATIONBASE_H
