#include "manipulationbase.h"

#include "tgt/qt/qttimer.h"

namespace voreen {

SpaceballEventListener::SpaceballEventListener(ManipulationBase* manipulation)
    : EventListener()
    , manipulation_(manipulation) 
{}

void SpaceballEventListener::timerEvent(TimeEvent* e) {
    tgt::vec3 offset(0.0, 0.0, 0.0);
    tgt::mat4 transformMatrix = tgt::mat4::createIdentity();
    manipulation_->applyTransformation(offset, transformMatrix);
}

ManipulationBase::ManipulationBase()
    : Processor()
	, manipulationType_("manipulationType", "Manipulation type")
	, manipulationAxis_("manipulationAxis", "Manipulation axis")
	, invertDirection_("invertDirection", "Invert direction")
    , manipulationSlider_("manipulationSlider", "Manipulation slider", 0, -1, 1)
{
    manipulationType_.addOption("rotate", "Rotate");
	manipulationType_.addOption("move", "Move");
	
	manipulationAxis_.addOption("x", "X");
	manipulationAxis_.addOption("y", "Y");
	manipulationAxis_.addOption("z", "Z");
    
    manipulationSlider_.onChange(CallMemberAction<ManipulationBase>(this, &ManipulationBase::guiEventHandler));

    addProperty(manipulationType_);
    addProperty(manipulationAxis_);
    addProperty(invertDirection_);
    addProperty(manipulationSlider_);
    
    spaceballListener_ = new SpaceballEventListener(this);
    eventHandler_ = new EventHandler();
    eventHandler_->addListenerToFront(spaceballListener_);
    
    timer_ = new QtTimer(eventHandler_);
    timer_->start(500);
}

ManipulationBase::~ManipulationBase() {
    timer_->stop();
    eventHandler_->clear();
    
    delete spaceballListener_;
    delete eventHandler_;
    delete timer_;
}

Processor* ManipulationBase::create() const {
    return new ManipulationBase();
}

void ManipulationBase::applyTransformation(tgt::vec3 offset, tgt::mat4 matrix) {
    //std::cout << offset << std::endl;
    //std::cout << matrix << std::endl;
}

// private methods
//
void ManipulationBase::guiEventHandler() {
    tgt::vec3 offset(0.0, 0.0, 0.0);
    tgt::mat4 transformMatrix = tgt::mat4::createIdentity();

    if (manipulationType_.get() == "move") {
        float offsetSize = 10;
        if (invertDirection_.get() == true) offsetSize = -offsetSize;
        
        /**/ if (manipulationAxis_.get() == "x") offset[0] += offsetSize;
        else if (manipulationAxis_.get() == "y") offset[1] += offsetSize;
        else if (manipulationAxis_.get() == "z") offset[2] += offsetSize;
    }
    else if (manipulationType_.get() == "rotate") {
        float angle = 10.f * 3.141592f / 180.f;
        if (invertDirection_.get() == true) angle = - angle;
        float sina = sin(angle);
        float cosa = cos(angle);
        
        /**/ if (manipulationAxis_.get() == "x")
            transformMatrix = tgt::mat4(
                1.0f, 0.0f, 0.0f, 0.0f, 
                0.0f, cosa,-sina, 0.0f, 
                0.0f, sina, cosa, 0.0f, 
                0.0f, 0.0f, 0.0f, 1.0f
            );
        else if (manipulationAxis_.get() == "y")
            transformMatrix = tgt::mat4(
                cosa, 0.0f,-sina, 0.0f, 
                0.0f, 1.0f, 0.0f, 0.0f, 
                sina, 0.0f, cosa, 0.0f, 
                0.0f, 0.0f, 0.0f, 1.0f
            );
        else if (manipulationAxis_.get() == "z")
            transformMatrix = tgt::mat4(
                cosa,-sina, 0.0f, 0.0f, 
                sina, cosa, 0.0f, 0.0f, 
                0.0f, 0.0f, 1.0f, 0.0f, 
                0.0f, 0.0f, 0.0f, 1.0f
            );
    }
    
    applyTransformation(offset, transformMatrix);
}

} // namespace
