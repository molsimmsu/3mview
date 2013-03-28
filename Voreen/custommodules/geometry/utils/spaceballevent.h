#ifndef SPACEBALLEVENT_H
#define SPACEBALLEVENT_H

#include <QInputEvent>
#include <spnav.h>
#include <QX11Info>

#include <vector>

#include "../processors/manipulationbase.h"
#define SQ(x)	((x) * (x))

class SpaceballEventListener;

    class SpaceballRegistrator
    {
        public:
            static SpaceballRegistrator* getInstance()
            {                  
                return instance_;
            }
            
            static void setInstance(SpaceballRegistrator* instance)
            {                
                instance_ = instance;
            }
            
            SpaceballRegistrator() {};   
            
            void addListener(SpaceballEventListener* listener) {
                listeners_.push_back(listener);
            }
            
            size_t getNumListeners() const {
                return listeners_.size();
            }
            
            SpaceballEventListener* getListener(size_t i) const {
                return listeners_[i];
            }
            
        private:
            static SpaceballRegistrator*    instance_;
            
                            // Constructor? (the {} brackets) are needed here.
            // Dont forget to declare these two. You want to make sure they
            // are unaccessable otherwise you may accidently get copies of
            // your singleton appearing.
            SpaceballRegistrator(SpaceballRegistrator const&);              // Don't Implement
            void operator=(SpaceballRegistrator const&); // Don't implement
            
            std::vector<SpaceballEventListener*> listeners_;
    };

namespace Spaceball
{

    enum ButtonStateType {BUTTON_NONE = 0, BUTTON_PRESSED, BUTTON_RELEASED};

    class EventBase : public QInputEvent
    {
    public:
        bool isHandled(){return handled;}
        void setHandled(bool sig){handled = sig;}

    protected:
        EventBase(QEvent::Type event);
        bool handled;
    };

    class MotionEvent : public EventBase
    {
    public:
        MotionEvent();
        MotionEvent(const MotionEvent& in);
        void translations(int &xTransOut, int &yTransOut, int &zTransOut);
        void setTranslations(const int &xTransIn, const int &yTransIn, const int &zTransIn);
        int translationX(){return xTrans;}
        int translationY(){return yTrans;}
        int translationZ(){return zTrans;}

        void rotations(int &xRotOut, int &yRotOut, int &zRotOut);
        void setRotations(const int &xRotIn, const int &yRotIn, const int &zRotIn);
        int rotationX(){return xRot;}
        int rotationY(){return yRot;}
        int rotationZ(){return zRot;}
        
        void makeRotationMatrixFromInput(float tx, float ty, float tz, float rx, float ry, float rz);
        tgt::mat4 getRotationMatrix(){return rotMat;}
        void setRotationMatrix(const tgt::mat4 &matrix);

        static int MotionEventType;

    private:
        int xTrans;
        int yTrans;
        int zTrans;
        int xRot;
        int yRot;
        int zRot;
        tgt::mat4 rotMat;
        bool handled;
        float transSens;
        float rotSens;
    };

    class ButtonEvent : public EventBase
    {
    public:
        ButtonEvent();
        ButtonEvent(const ButtonEvent& in);
        ButtonStateType buttonStatus();
        void setButtonStatus(const ButtonStateType &buttonStatusIn);
        int buttonNumber();
        void setButtonNumber(const int &buttonNumberIn);

        static int ButtonEventType;

    private:
        ButtonStateType buttonState;
        int button;
    };
}

class SpaceballEventListener {
public:
    SpaceballEventListener(voreen::ManipulationBase* manipulation)
      : manipulation_(manipulation)
    { }
    
        
    bool event(Spaceball::MotionEvent* motionEvent) {
        std::cout << motionEvent->getRotationMatrix() << std::endl;
        tgt::vec3 offset(0.0, 0.0, 0.0);
        tgt::mat4 transformMatrix = manipulation_->GetInvMat() * motionEvent->getRotationMatrix() * manipulation_->GetRotMat();
        manipulation_->applyTransformation(offset, transformMatrix);
    }
    
private:
    voreen::ManipulationBase* manipulation_;
};
#endif // SPACEBALLEVENT_H
