
#include "spaceballevent.h"


using namespace Spaceball;

int MotionEvent::MotionEventType = QEvent::registerEventType();
int ButtonEvent::ButtonEventType = QEvent::registerEventType();

EventBase::EventBase(QEvent::Type event) : QInputEvent(static_cast<QEvent::Type>(event)), handled(false)
{

}

MotionEvent::MotionEvent() : EventBase(static_cast<QEvent::Type>(MotionEventType)),
    xTrans(0), yTrans(0), zTrans(0), xRot(0), yRot(0), zRot(0), transSens(0.01), rotSens(0.001), rotMat(tgt::mat4::createIdentity())
{
}

MotionEvent::MotionEvent(const MotionEvent& in) : EventBase(static_cast<QEvent::Type>(MotionEventType))
{
    xTrans  = in.xTrans;
    yTrans  = in.yTrans;
    zTrans  = in.zTrans;
    xRot    = in.xRot;
    yRot    = in.yRot;
    zRot    = in.zRot;
    handled = in.handled;
}

void MotionEvent::translations(int &xTransOut, int &yTransOut, int &zTransOut)
{
    xTransOut = xTrans;
    yTransOut = yTrans;
    zTransOut = zTrans;
}

void MotionEvent::makeRotationMatrixFromInput(float tx, float ty, float tz, float rx, float ry, float rz)
{

    // calculates the ammount of deviation from zero
    float axis_len = sqrt(SQ(rx) + SQ(ry) + SQ(rz));                
    tgt::quat rotquat;
    float half_angle = axis_len * 0.5 * rotSens;
    float sin_half = sin(half_angle);
    //forms a quaternion
    rotquat.w = cos(half_angle);
    rotquat.x = -rx / axis_len * sin_half;
    rotquat.y = -ry / axis_len * sin_half;
    rotquat.z = rz / axis_len * sin_half;
    //rotation matrix from quaternion
    tgt::mat4 transformMatrix = tgt::generateMatrixFromQuat(rotquat);
    //add translation
    transformMatrix[0][3]=tx * transSens;
    transformMatrix[1][3]=ty * transSens;
    transformMatrix[2][3]=-tz * transSens;
    rotMat = transformMatrix;
}

void MotionEvent::setRotationMatrix(const tgt::mat4 &matrix)
{
    rotMat = matrix;
}

void MotionEvent::setTranslations(const int &xTransIn, const int &yTransIn, const int &zTransIn)
{
    xTrans = xTransIn;
    yTrans = yTransIn;
    zTrans = zTransIn;
}

void MotionEvent::rotations(int &xRotOut, int &yRotOut, int &zRotOut)
{
    xRotOut = xRot;
    yRotOut = yRot;
    zRotOut = zRot;
}

void MotionEvent::setRotations(const int &xRotIn, const int &yRotIn, const int &zRotIn)
{
    xRot = xRotIn;
    yRot = yRotIn;
    zRot = zRotIn;
}


ButtonEvent::ButtonEvent() : EventBase(static_cast<QEvent::Type>(ButtonEventType)),
    buttonState(BUTTON_NONE), button(0)
{
}

ButtonEvent::ButtonEvent(const ButtonEvent& in) : EventBase(static_cast<QEvent::Type>(ButtonEventType))
{
    buttonState = in.buttonState;
    button = in.button;
    handled = in.handled;
}

ButtonStateType ButtonEvent::buttonStatus()
{
    return buttonState;
}

void ButtonEvent::setButtonStatus(const ButtonStateType &buttonStatusIn)
{
    buttonState = buttonStatusIn;
}

int ButtonEvent::buttonNumber()
{
    return button;
}

void ButtonEvent::setButtonNumber(const int &buttonNumberIn)
{
    button = buttonNumberIn;
}
