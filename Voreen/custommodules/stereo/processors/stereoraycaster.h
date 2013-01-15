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

#ifndef VRN_STEREORAYCASTER_H
#define VRN_STEREORAYCASTER_H

#include "voreen/core/processors/volumeraycaster.h"

#include "voreen/core/properties/cameraproperty.h"
#include "voreen/core/properties/transfuncproperty.h"
#include "voreen/core/properties/shaderproperty.h"
#include "voreen/core/properties/floatproperty.h"

namespace voreen {

/**
 * Performs a simple single pass raycasting without lighting, which can be modified through a shader property.
 */
class StereoRaycaster : public VolumeRaycaster {
public:
    StereoRaycaster();
    virtual Processor* create() const;

    virtual std::string getCategory() const   { return "Raycasting";      }
    virtual std::string getClassName() const  { return "StereoRaycaster"; }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE; }

protected:
    virtual void setDescriptions() {
        setDescription("Performs a simple single pass raycasting without lighting, which can be modified through a shader property.");
    }

    virtual void beforeProcess();
    virtual void process();
    virtual void initialize() throw (tgt::Exception);
    virtual void deinitialize() throw (tgt::Exception);
    
    virtual void render(RenderPort& outport, RenderPort& entryPort, RenderPort& exitPort, CameraProperty camera);

    virtual void rebuildShader();
    virtual std::string generateHeader();

private:
    VolumePort volumePort_;
    
    RenderPort entryPortLeft_;
    RenderPort exitPortLeft_;
    
    RenderPort entryPortRight_;
    RenderPort exitPortRight_;
    
    RenderPort outportLeft_;
    RenderPort outportRight_;

    ShaderProperty shader_;           ///< the property that stores the used shader
    TransFuncProperty transferFunc_;  ///< the property that controls the transfer function
    CameraProperty camera_;           ///< necessary for depth value calculation
    CameraProperty cameraLeft_;       ///< left camera which is synchronyzed with MeshEntryExitPoints camera
    CameraProperty cameraRight_;      ///< right camera which is synchronyzed with MeshEntryExitPoints camera
    FloatProperty eyeSeparation_;     ///< stereo eye separation
};


} // namespace

#endif // VRN_STEREORAYCASTER_H
