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

#ifndef VRN_STEREOMESHENTRYEXITPOINTS_H
#define VRN_STEREOMESHENTRYEXITPOINTS_H

#include "voreen/core/processors/renderprocessor.h"
#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/cameraproperty.h"

#include "tgt/shadermanager.h"

namespace voreen {

class StereoCameraInteractionHandler;

/**
 * Calculates the entry and exit points for GPU raycasting and stores them in textures.
 *
 * @see CubeMeshProxyGeometry, SingleVolumeRaycaster
 */
class StereoMeshEntryExitPoints : public RenderProcessor {
public:
    StereoMeshEntryExitPoints();
    virtual ~StereoMeshEntryExitPoints();
    virtual Processor* create() const;

    virtual std::string getClassName() const    { return "StereoMeshEntryExitPoints"; }
    virtual std::string getCategory() const     { return "Entry-Exit Points";   }
    virtual CodeState getCodeState() const      { return CODE_STATE_STABLE; }

    virtual bool isReady() const;

protected:
    virtual void setDescriptions() {
        setDescription("This is the standard processor for generating entry- and exit-points within Voreen. The generated image color-codes the ray parameters for a subsequent VolumeRaycaster. See also: CubeMeshProxyGeometry\
<p><span style=\"font-weight: bold\">Note:</span> The input proxy geometry is expected to be convex. Non-convex geometry is accepted, but may yield unexpected results.</p>\
");
    }

    virtual void beforeProcess();
    virtual void process();
    virtual void initialize() throw (tgt::Exception);
    virtual void deinitialize() throw (tgt::Exception);

    /**
     *  Jitters entry points in ray direction.
     *  Entry and Exit Params have to be generated before
     *  calling this method.
     */
    void jitterEntryPoints();

    void onJitterEntryPointsChanged();

    void adaptToGeometry();

    // ports
    RenderPort entryPort_;
    RenderPort exitPort_;
    GeometryPort inport_;
    RenderPort tmpPort_;

    // properties
    BoolProperty supportCameraInsideVolume_;
    BoolProperty jitterEntryPoints_;
    BoolProperty useFloatRenderTargets_;
    BoolProperty useCulling_;
    FloatProperty jitterStepLength_;

    CameraProperty camera_;  ///< camera used for rendering the proxy geometry
    CameraProperty cameraLeft_;  ///< camera used for rendering the proxy geometry
    CameraProperty cameraRight_;  ///< camera used for rendering the proxy geometry

    FloatProperty eyeSeparation_; ///< separation of the left and right camera
    FloatProperty cameraDistance_; ///< distance of the camera to the projection plane

    // interaction handlers
    StereoCameraInteractionHandler* cameraHandler_;

    tgt::Shader* shaderProgram_;
    tgt::Shader* shaderProgramJitter_;

    MeshListGeometry geometry_;

    /// Category used for logging.
    static const std::string loggerCat_;
};

} // namespace voreen

#endif //VRN_STEREOMESHENTRYEXITPOINTS_H
