/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2012 University of Muenster, Germany.                        *
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>        *
 * For a list of authors please refer to the file "CREDITS.txt".                   *
 *                                                                                 *
 * This file is part of the Voreen software package. Voreen is free software:      *
 * you can redistribute it and/or modify it under the terms of the GNU General     *
 * Public License version 2 as published by the Free Software Foundation.          *
 *                                                                                 *
 * Voreen is distributed in the hope that it will be useful, but WITHOUT ANY       *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR   *
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.      *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License in the file   *
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                                 *
 * For non-commercial academic use see the license exception specified in the file *
 * "LICENSE-academic.txt". To get information about commercial licensing please    *
 * contact the authors.                                                            *
 *                                                                                 *
 ***********************************************************************************/

#include "stereocamerainteractionhandler.h"
#include "voreen/core/voreenapplication.h"
#include "voreen/core/interaction/trackballnavigation.h"
#include "voreen/core/interaction/firstpersonnavigation.h"
#include "voreen/core/properties/cameraproperty.h"

#include "tgt/event/mouseevent.h"
#include "tgt/event/keyevent.h"
#include "tgt/event/timeevent.h"
#include "tgt/timer.h"
#include "voreen/core/properties/eventproperty.h"

using tgt::Event;
using tgt::MouseEvent;
using tgt::KeyEvent;
using tgt::TimeEvent;

namespace voreen {

StereoCameraInteractionHandler::StereoCameraInteractionHandler() :
    InteractionHandler("dummy", "dummy"),
    navigationMetaphor_("naviMetaphor", "Navigation Metaphor", Processor::VALID)
{}

StereoCameraInteractionHandler::StereoCameraInteractionHandler(const std::string& id, const std::string& guiName,
              CameraProperty* cameraProp, CameraProperty* cameraPropLeft,
              CameraProperty* cameraPropRight, FloatProperty* eyeSeparation, FloatProperty* cameraDistance, bool sharing, bool enabled)
    : InteractionHandler(id, guiName)
    , cameraProp_(cameraProp)
    , cameraPropLeft_(cameraPropLeft)
    , cameraPropRight_(cameraPropRight)
    , eyeSeparation_(eyeSeparation)
    , cameraDistance_(cameraDistance)
    , navigationMetaphor_(id + ".interactionMetaphor", guiName + " Interaction", Processor::VALID)
    , shiftTrackballCenter_(id + ".shiftTrackballCenter", "Rotate trackball around...", Processor::VALID)
    , adjustCamera_(id + ".adjustCamera", "Adapt camera to scene size...", Processor::VALID)
    , resetTrackballCenter_(id + ".resetTrackballCenter", "Reset Trackball Center")
    , currentSceneMesh_(MeshListGeometry())
{
    tgtAssert(cameraProp, "No camera property");
    cameraProp_ = cameraProp;

    navigationMetaphor_.addOption("trackball",     "Trackball",     TRACKBALL);
    navigationMetaphor_.addOption("first-person",  "First-Person",  FIRST_PERSON);
    navigationMetaphor_.select("trackball");
    navigationMetaphor_.onChange(CallMemberAction<StereoCameraInteractionHandler>(this, &StereoCameraInteractionHandler::adjustWidgetStates));
    addProperty(navigationMetaphor_);

    shiftTrackballCenter_.addOption("origin",      "World origin");
    shiftTrackballCenter_.addOption("scenecenter", "Scene center");
    shiftTrackballCenter_.addOption("shift",       "Camera shift");
    shiftTrackballCenter_.select("scenecenter");
    shiftTrackballCenter_.onChange(CallMemberAction<StereoCameraInteractionHandler>(this, &StereoCameraInteractionHandler::adjustCenterShift));
    addProperty(shiftTrackballCenter_);

    adjustCamera_.addOption("never", "Never");
    adjustCamera_.addOption("bigsizechange", "Only on large difference");
    adjustCamera_.addOption("always", "Always");
    adjustCamera_.select("bigsizechange");
    addProperty(adjustCamera_);

    resetTrackballCenter_.onChange(CallMemberAction<StereoCameraInteractionHandler>(this, &StereoCameraInteractionHandler::resetTrackballCenter));
    addProperty(resetTrackballCenter_);

    // navigations
    tbNavi_ = new TrackballNavigation(cameraProp_, TrackballNavigation::ROTATE_MODE);
    fpNavi_ = new FirstPersonNavigation(cameraProp_);

    // event properties trackball
    rotateEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".rotate", guiName + " Rotate", this,
        &StereoCameraInteractionHandler::rotateEvent,
        MouseEvent::MOUSE_BUTTON_LEFT,
        MouseEvent::ACTION_ALL,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    addEventProperty(rotateEvent_);

    zoomEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".zoom", guiName + " Zoom", this,
        &StereoCameraInteractionHandler::zoomEvent,
        MouseEvent::MOUSE_BUTTON_RIGHT,
        MouseEvent::ACTION_ALL,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    addEventProperty(zoomEvent_);

    shiftEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".shift", guiName + " Shift", this,
        &StereoCameraInteractionHandler::shiftEvent,
        MouseEvent::MOUSE_BUTTON_LEFT,
        MouseEvent::ACTION_ALL,
        tgt::Event::SHIFT, sharing, enabled);
    addEventProperty(shiftEvent_);

    wheelZoomEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".wheelZoom", guiName + " Wheel Zoom", this,
        &StereoCameraInteractionHandler::zoomEvent,
        MouseEvent::MOUSE_WHEEL,
        MouseEvent::WHEEL,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    addEventProperty(wheelZoomEvent_);

    // event properties first-person navigation
    moveForwardEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".moveForward", guiName + " Move Forward", this,
        &StereoCameraInteractionHandler::keyEvent,
        tgt::KeyEvent::K_W,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    moveForwardEvent_->onChange(&StereoCameraInteractionHandler::updateFpKeySettings);
    addEventProperty(moveForwardEvent_);

    moveLeftEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".moveLeft", guiName + " Move Left", this,
        &StereoCameraInteractionHandler::keyEvent,
        tgt::KeyEvent::K_A,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    moveLeftEvent_->onChange(&StereoCameraInteractionHandler::updateFpKeySettings);
    addEventProperty(moveLeftEvent_);

    moveBackwardEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".moveBackward", guiName + " Move Backward", this,
        &StereoCameraInteractionHandler::keyEvent,
        tgt::KeyEvent::K_S,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    moveBackwardEvent_->onChange(&StereoCameraInteractionHandler::updateFpKeySettings);
    addEventProperty(moveBackwardEvent_);

    moveRightEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".moveRight", guiName + " Move Right", this,
        &StereoCameraInteractionHandler::keyEvent,
        tgt::KeyEvent::K_D,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    moveRightEvent_->onChange(&StereoCameraInteractionHandler::updateFpKeySettings);
    addEventProperty(moveRightEvent_);

    moveUpEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".moveUp", guiName + " Move Up", this,
        &StereoCameraInteractionHandler::keyEvent,
        tgt::KeyEvent::K_R,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    moveUpEvent_->onChange(&StereoCameraInteractionHandler::updateFpKeySettings);
    addEventProperty(moveUpEvent_);

    moveDownEvent_ = new EventProperty<StereoCameraInteractionHandler>(id + ".moveDown", guiName + " Move Down", this,
        &StereoCameraInteractionHandler::keyEvent,
        tgt::KeyEvent::K_F,
        tgt::Event::MODIFIER_NONE, sharing, enabled);
    moveDownEvent_->onChange(&StereoCameraInteractionHandler::updateFpKeySettings);
    addEventProperty(moveDownEvent_);

    // timer
    timerEventHandler_ = new tgt::EventHandler;
    motionTimer_ = VoreenApplication::app()->createTimer(timerEventHandler_);
    tgtAssert(timerEventHandler_, "No timerEventHandler");
    timerEventHandler_->addListenerToFront(this);

    adjustWidgetStates();
}

StereoCameraInteractionHandler::~StereoCameraInteractionHandler() {

    // event properties are deleted by base class InteractionHandler

    delete tbNavi_;
    delete fpNavi_;
    delete timerEventHandler_;
    delete motionTimer_;
}

void StereoCameraInteractionHandler::updateStereo() {
    tgt::Camera cam = cameraProp_->get();
    tgt::Camera camLeft = cameraPropLeft_->get();
    tgt::Camera camRight = cameraPropRight_->get();
    
    tgt::vec3 strafe = cam.getStrafe() * eyeSeparation_->get();
    
    camLeft.positionCamera(cam.getPosition() - strafe, cam.getFocus() - strafe, cam.getUpVector());
    camRight.positionCamera(cam.getPosition() + strafe, cam.getFocus() + strafe, cam.getUpVector());
    
    tgt::Frustum camFrust = cam.getFrustum();
    float left = camFrust.getLeft();
    float right = camFrust.getRight();
    float delta = 0.5 * eyeSeparation_->get();
    
    camLeft.setFovy(cam.getFovy());
    camLeft.setRatio(cam.getRatio());
    camLeft.setNearDist(cam.getNearDist());
    camLeft.setFarDist(cam.getFarDist());
    
    camRight.setFovy(cam.getFovy());
    camRight.setRatio(cam.getRatio());
    camRight.setNearDist(cam.getNearDist());
    camRight.setFarDist(cam.getFarDist());
    
    tgt::Frustum camLeftFrust = camFrust;
    camLeftFrust.setLeft(left + delta);
    camLeftFrust.setRight(right + delta);
    
    tgt::Frustum camRightFrust = camFrust;
    camRightFrust.setLeft(left - delta);
    camRightFrust.setRight(right - delta);
    
    cameraPropLeft_->set(camLeft);
    cameraPropRight_->set(camRight);
}

// TODO: left clicked mousedragging is catched by fpNavi, too. Maybe this method should get a proper name?!
void StereoCameraInteractionHandler::rotateEvent(tgt::MouseEvent* e) {
    if (navigationMetaphor_.isSelected("trackball")){
        tgtAssert(cameraProp_, "No camera property");
        tgtAssert(tbNavi_, "No trackball navigation");

        //TODO: remove if it works (stefan)
        //// assign new camera object to navigation, if it has changed
        //if (tbNavi_->getTrackball()->getCamera() != cameraProp_->get()) {
            //tbNavi_->getTrackball()->setCamera(cameraProp_->get());
        //}

        // propagate event to trackball navigation
        if (e->action() == MouseEvent::PRESSED) {
            cameraProp_->toggleInteractionMode(true, this);
            tbNavi_->mousePressEvent(e);
        }
        else if (e->action() == MouseEvent::RELEASED) {
            cameraProp_->toggleInteractionMode(false, this);
            tbNavi_->mouseReleaseEvent(e);
        }
        else if (e->action() == MouseEvent::MOTION)
            tbNavi_->mouseMoveEvent(e);
        else if (e->action() == MouseEvent::DOUBLECLICK)
            tbNavi_->mouseDoubleClickEvent(e);
        else if (e->action() == MouseEvent::WHEEL)
            tbNavi_->wheelEvent(e);
    }
    else if (navigationMetaphor_.isSelected("first-person")){
        if (e->action() == MouseEvent::PRESSED){
            cameraProp_->toggleInteractionMode(true, this);
            fpNavi_->mousePressEvent(e);
        }
        else if (e->action() == MouseEvent::RELEASED){
            if (!fpNavi_->isMoving())
                cameraProp_->toggleInteractionMode(false, this);

            e->accept();
        }
        else if (e->action() == MouseEvent::MOTION){
            motionTimer_->stop();

            fpNavi_->mouseMoveEvent(e);

            // Restart motionTimer_, if necessary
            if (fpNavi_->isMoving())
                motionTimer_->start(40, 0);
        }
    }

    // invalidate processor and update camera prop widgets, if event has been accepted
    if (e->isAccepted()) {
        cameraProp_->invalidate();
    }
    
    updateStereo();
}

// TODO: right clicked mousedragging is catched by fpNavi, too. Maybe this method should get a proper name?!
void StereoCameraInteractionHandler::zoomEvent(tgt::MouseEvent* e) {
    if (navigationMetaphor_.isSelected("trackball")){
        tgtAssert(cameraProp_, "No camera property");
        tgtAssert(tbNavi_, "No trackball navigation");

        //TODO: remove if it works (stefan)
        // assign new camera object to navigation, if it has changed
        //if (tbNavi_->getTrackball()->getCamera() != cameraProp_->get()) {
            //tbNavi_->getTrackball()->setCamera(cameraProp_->get());
        //}
        tbNavi_->setMode(TrackballNavigation::ZOOM_MODE);

        // propagate event to trackball navigation
        if (e->action() == MouseEvent::PRESSED) {
            cameraProp_->toggleInteractionMode(true, this);
            tbNavi_->mousePressEvent(e);
        }
        else if (e->action() == MouseEvent::RELEASED) {
            cameraProp_->toggleInteractionMode(false, this);
            tbNavi_->mouseReleaseEvent(e);
        }
        else if (e->action() == MouseEvent::MOTION)
            tbNavi_->mouseMoveEvent(e);
        else if (e->action() == MouseEvent::DOUBLECLICK)
            tbNavi_->mouseDoubleClickEvent(e);
        else if (e->action() == MouseEvent::WHEEL)
            tbNavi_->wheelEvent(e);

        tbNavi_->setMode(TrackballNavigation::ROTATE_MODE);
    }
    else if (navigationMetaphor_.isSelected("first-person")){
        if (e->action() == MouseEvent::PRESSED){
            cameraProp_->toggleInteractionMode(true, this);
            fpNavi_->mousePressEvent(e);
        }
        else if (e->action() == MouseEvent::RELEASED){
            cameraProp_->toggleInteractionMode(false, this);
            e->accept();
        }
        else if (e->action() == MouseEvent::MOTION){
            motionTimer_->stop();

            fpNavi_->mouseMoveEvent(e);

            // Restart motionTimer_, if necessary
            if (fpNavi_->isMoving())
                motionTimer_->start(40, 0);
        }
    }

    // invalidate processor and update camera prop widgets, if event has been accepted
    if (e->isAccepted()) {
        cameraProp_->invalidate();
    }
    
    updateStereo();
}

void StereoCameraInteractionHandler::shiftEvent(tgt::MouseEvent* e) {
    if (navigationMetaphor_.isSelected("trackball")){
        tgtAssert(cameraProp_, "No camera property");
        tgtAssert(tbNavi_, "No trackball navigation");

        //TODO: remove if it works (stefan)
        // assign new camera object to navigation, if it has changed
        //if (tbNavi_->getTrackball()->getCamera() != cameraProp_->get()) {
            //tbNavi_->getTrackball()->setCamera(cameraProp_->get());
        //}

        tbNavi_->setMode(TrackballNavigation::SHIFT_MODE);

        // propagate event to trackball navigation
        if (e->action() == MouseEvent::PRESSED) {
            cameraProp_->toggleInteractionMode(true, this);
            tbNavi_->mousePressEvent(e);
        }
        else if (e->action() == MouseEvent::RELEASED) {
            cameraProp_->toggleInteractionMode(false, this);
            tbNavi_->mouseReleaseEvent(e);
        }
        else if (e->action() == MouseEvent::MOTION)
            tbNavi_->mouseMoveEvent(e);
        else if (e->action() == MouseEvent::DOUBLECLICK)
            tbNavi_->mouseDoubleClickEvent(e);
        else if (e->action() == MouseEvent::WHEEL)
            tbNavi_->wheelEvent(e);

        tbNavi_->setMode(TrackballNavigation::ROTATE_MODE);
    }
    else if (navigationMetaphor_.isSelected("first-person")){
        // Hack: prevents camerainteraction handler from executing shift events, if fpNavi is activated
        e->accept();
    }

    // invalidate processor and update camera prop widgets, if event has been accepted
    if (e->isAccepted()) {
        cameraProp_->invalidate();
    }
    
    updateStereo();
}

void StereoCameraInteractionHandler::keyEvent(tgt::KeyEvent* e){
    tgtAssert(cameraProp_, "No camera property");

    tgtAssert(fpNavi_, "No first person Navigation");

    if (navigationMetaphor_.isSelected("first-person")){
        fpNavi_->keyEvent(e);

        if (e->pressed()){
            if (fpNavi_->isMoving()){
                cameraProp_->toggleInteractionMode(true, this);
                motionTimer_->start(40, 0);
            }
            else
                 motionTimer_->stop();
        }
        else{
            if (!fpNavi_->isMoving()){
                cameraProp_->toggleInteractionMode(pressedMouseButtons_.any(), this);
                    motionTimer_->stop();
            }
        }

        e->accept();
    }
}

void StereoCameraInteractionHandler::onEvent(Event* eve) {
    tgtAssert(cameraProp_, "No camera property");
    tgtAssert(fpNavi_, "No first person Navigation");

    if (navigationMetaphor_.isSelected("first-person")){
        if (tgt::TimeEvent* timeEve = dynamic_cast<tgt::TimeEvent*>(eve)){
            if (fpNavi_->isMoving()){
                cameraProp_->toggleInteractionMode(true, this);
                fpNavi_->timerEvent(timeEve);

                eve->accept();
            }
        }
    }

    // invalidate processor and update camera prop widgets, if event has been accepted
    if (eve->isAccepted()) {
        cameraProp_->invalidate();
    }
}

void StereoCameraInteractionHandler::setNavigationMode(NavigationMetaphor behaviour){
    navigationMetaphor_.selectByValue(behaviour);
}

StereoCameraInteractionHandler::NavigationMetaphor StereoCameraInteractionHandler::getNavigationMode() const {
    return navigationMetaphor_.getValue();
}

void StereoCameraInteractionHandler::updateFpKeySettings() {
    tgtAssert(fpNavi_, "no first-person navigation");
    tgtAssert(moveForwardEvent_ && moveLeftEvent_ && moveBackwardEvent_ , "missing event property");
    tgtAssert(moveRightEvent_ && moveUpEvent_ && moveDownEvent_ , "missing event property");

    fpNavi_->setKeysetting(moveForwardEvent_->getKeyCode(), moveBackwardEvent_->getKeyCode(),
        moveLeftEvent_->getKeyCode(), moveRightEvent_->getKeyCode(),
        moveUpEvent_->getKeyCode(), moveDownEvent_->getKeyCode());
}

void StereoCameraInteractionHandler::adjustWidgetStates() {
    tgtAssert(rotateEvent_ && zoomEvent_ && shiftEvent_ && wheelZoomEvent_, "missing event property");
    tgtAssert(moveForwardEvent_ && moveLeftEvent_ && moveBackwardEvent_ , "missing event property");
    tgtAssert(moveRightEvent_ && moveUpEvent_ && moveDownEvent_ , "missing event property");

    bool trackballSelected = navigationMetaphor_.isSelected("trackball");
    bool firstPersonSelected = navigationMetaphor_.isSelected("first-person");

    // trackball properties
    shiftTrackballCenter_.setVisible(trackballSelected);
    resetTrackballCenter_.setVisible(trackballSelected);
    rotateEvent_->setVisible(trackballSelected);
    zoomEvent_->setVisible(trackballSelected);
    shiftEvent_->setVisible(trackballSelected);
    wheelZoomEvent_->setVisible(trackballSelected);

    // first-person properties
    moveForwardEvent_->setVisible(firstPersonSelected);
    moveLeftEvent_->setVisible(firstPersonSelected);
    moveBackwardEvent_->setVisible(firstPersonSelected);
    moveRightEvent_->setVisible(firstPersonSelected);
    moveUpEvent_->setVisible(firstPersonSelected);
    moveDownEvent_->setVisible(firstPersonSelected);
}

void StereoCameraInteractionHandler::adjustCenterShift() {
    if(shiftTrackballCenter_.isSelected("origin")) {
        cameraProp_->getTrackball()->setMoveCenter(false);
    } else if (shiftTrackballCenter_.isSelected("scenecenter")) {
        cameraProp_->getTrackball()->setMoveCenter(false);
        tgt::Bounds bBox(tgt::vec3(0.f));
        if (!currentSceneMesh_.empty())
            bBox = currentSceneMesh_.getBoundingBox();
        cameraProp_->getTrackball()->setCenter(bBox.center());
    } else if (shiftTrackballCenter_.isSelected("shift")) {
        cameraProp_->getTrackball()->setMoveCenter(true);
    }
}

void StereoCameraInteractionHandler::resetTrackballCenter() {
    tgt::Bounds bBox(tgt::vec3(0.f));
    if (!currentSceneMesh_.empty())
        bBox = currentSceneMesh_.getBoundingBox();
    tgt::Camera cam = cameraProp_->get();
    cam.setPosition(bBox.center() - cam.getFocalLength() * cam.getLook());
    cam.setFocus(bBox.center());
    cameraProp_->set(cam);
    cameraProp_->getTrackball()->setCenter(bBox.center());
}

void StereoCameraInteractionHandler::setVisible(bool state) {
    InteractionHandler::setVisible(state);
    navigationMetaphor_.setVisible(state);
    if (state)
        adjustWidgetStates();
}

void StereoCameraInteractionHandler::adaptInteractionToScene(const MeshListGeometry& geometry) {
    if(geometry.empty())
        return;

    tgt::Bounds bounds = geometry.getBoundingBox();

    tgt::vec3 extentOld = tgt::vec3(0.f);
    if (!currentSceneMesh_.empty()) {
        extentOld = currentSceneMesh_.getBoundingBox().diagonal();
    }

    currentSceneMesh_ = geometry;

    bool adaptMaxValues = true;
    if(hmul(extentOld) != 0.f) {
        if(adjustCamera_.isSelected("never"))
            adaptMaxValues = false;
        else if(adjustCamera_.isSelected("bigsizechange")) {
            // resize only if the size of the scene has drastically changed
            float relSize = hmul(extentOld) / hmul(bounds.diagonal());
            if(relSize > 0.2f && relSize < 5.f)
                adaptMaxValues = false;
        }
    } else {
        // always adapt far distance if there was no previous scene geometry
        tgt::Camera cam = cameraProp_->get();
        float newMaxDist = 250.f * tgt::max(bounds.diagonal());
        tbNavi_->setMaxDist(newMaxDist);
        cameraProp_->setMaxValue(newMaxDist);
        cam.setFarDist(std::max(cam.getFarDist(), newMaxDist + tgt::max(bounds.diagonal())));
        cameraProp_->set(cam);
        return;
    }

    tgt::Camera cam = cameraProp_->get();

    if(adaptMaxValues) {
        LINFOC("voreen.StereoCameraInteractionHandler", "Adapting camera handling to new scene size...");
        float oldRelCamDist = cam.getFocalLength() / tbNavi_->getMaxDist();
        float maxSideLength = tgt::max(bounds.diagonal());

        // The factor 250 is derived from an earlier constant maxDist of 500 and a constant maximum cubeSize element of 2
        float newMaxDist = 250.f * maxSideLength;
        float newAbsCamDist = oldRelCamDist * newMaxDist;

        if(shiftTrackballCenter_.isSelected("shift")) {
            tgt::vec3 newFocus = cam.getFocus() * (newAbsCamDist / cam.getFocalLength());
            tgt::vec3 newPos   = cam.getPosition() * (newAbsCamDist / cam.getFocalLength());
            cam.setFocus(newFocus);
            cam.setPosition(newPos);
        } else {
            tgt::vec3 newFocus = geometry.getBoundingBox().center();
            tgt::vec3 newPos   = geometry.getBoundingBox().center() - cam.getLook() * newAbsCamDist;
            cam.setFocus(newFocus);
            cam.setPosition(newPos);
        }
        tbNavi_->setMaxDist(newMaxDist);
        cameraProp_->setMaxValue(newMaxDist);
        cam.setFarDist(std::max(cam.getFarDist(), newMaxDist + maxSideLength));
        adjustCenterShift();

    }
    //else if(!shiftTrackballCenter_.isSelected("shift")) {
        //tgt::vec3 centerDiff = geometry.getBoundingBox().center() - cameraProp_->getTrackball()->getCenter();
        //if(...) {
            //adjustCenterShift();
            //tgt::vec3 newFocus = geometry.getBoundingBox().center();
            //tgt::vec3 newPos   = geometry.getBoundingBox().center() - cam.getLook() * cam.getFocalLength();
            //cam.setFocus(newFocus);
            //cam.setPosition(newPos);
        //}
    //}
    cameraProp_->set(cam);
}

} // namespace
