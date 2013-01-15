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

#include "stereoraycaster.h"

#include "tgt/textureunit.h"

using tgt::vec3;
using tgt::TextureUnit;

namespace voreen {

StereoRaycaster::StereoRaycaster()
    : VolumeRaycaster()
    , volumePort_(Port::INPORT, "volumehandle.volumehandle", "Volume Input")
    , entryPortLeft_(Port::INPORT, "image.entrypoints", "Entry-points Input", false, Processor::INVALID_RESULT, RenderPort::RENDERSIZE_ORIGIN)
    , exitPortLeft_(Port::INPORT, "image.exitpoints", "Exit-points Input", false, Processor::INVALID_RESULT, RenderPort::RENDERSIZE_ORIGIN)
    , entryPortRight_(Port::INPORT, "image.entrypoints1", "Entry-points Input", false, Processor::INVALID_RESULT, RenderPort::RENDERSIZE_ORIGIN)
    , exitPortRight_(Port::INPORT, "image.exitpoints1", "Exit-points Input", false, Processor::INVALID_RESULT, RenderPort::RENDERSIZE_ORIGIN)
    , outportLeft_(Port::OUTPORT, "image.output", "Image Left Output", true, Processor::INVALID_RESULT, RenderPort::RENDERSIZE_RECEIVER)
    , outportRight_(Port::OUTPORT, "image.output1", "Image Right Output", true, Processor::INVALID_RESULT, RenderPort::RENDERSIZE_RECEIVER)
    , shader_("shader", "Shader", "rc_simple.frag", "passthrough.vert")
    , transferFunc_("transferFunction", "Transfer Function", Processor::INVALID_RESULT,
        TransFuncProperty::Editors(TransFuncProperty::INTENSITY | TransFuncProperty::INTENSITY_RAMP))
    , camera_("camera", "Camera")
    , cameraLeft_("cameraLeft", "Camera Left")
    , cameraRight_("cameraRight", "Camera Right")
    , eyeSeparation_("eyeSeparation", "Eye separation", 0.f, 0.f, 10.f)
{
    volumePort_.showTextureAccessProperties(true);
    addPort(volumePort_);
    
    addPort(entryPortLeft_);
    addPort(exitPortLeft_);
    
    addPort(entryPortRight_);
    addPort(exitPortRight_);
    
    addPort(outportLeft_);
    addPort(outportRight_);

    addProperty(shader_);
    addProperty(transferFunc_);

    // camera is required for depth value calculation
    addProperty(camera_);
    addProperty(cameraLeft_);
    addProperty(cameraRight_);
    
    addProperty(eyeSeparation_);
}

Processor* StereoRaycaster::create() const {
    return new StereoRaycaster();
}

void StereoRaycaster::initialize() throw (tgt::Exception) {
    VolumeRaycaster::initialize();
    rebuildShader();
}

void StereoRaycaster::deinitialize() throw (tgt::Exception) {
    VolumeRaycaster::deinitialize();
}

void StereoRaycaster::beforeProcess() {
    VolumeRaycaster::beforeProcess();

    // rebuild shader, if changed
    if (getInvalidationLevel() >= Processor::INVALID_PROGRAM)
        rebuildShader();

    // assign volume to transfer function
    transferFunc_.setVolumeHandle(volumePort_.getData());
    LGL_ERROR;
}

void StereoRaycaster::process() {
    float es = eyeSeparation_.get();

    tgt::Camera cam = camera_.get();
    tgt::vec3 pos = cam.getPosition();
    tgt::vec3 offset(es, es, es);
    
    const tgt::Camera camLeft = *cam.clone();
    const tgt::Camera camRight = *cam.clone();
    
    cameraLeft_.set(camLeft);
    cameraRight_.set(camRight);

    cameraLeft_.setPosition(pos + offset);
    cameraRight_.setPosition(pos - offset);

	if (outportLeft_.isReady() && entryPortLeft_.isReady() && exitPortLeft_.isReady()) 
		render(outportLeft_, entryPortLeft_, exitPortLeft_, camera_);
		
	if (outportRight_.isReady() && entryPortRight_.isReady() && exitPortRight_.isReady()) 
		render(outportRight_, entryPortRight_, exitPortRight_, camera_);
}

void StereoRaycaster::render(RenderPort& outport, RenderPort& entryPort, RenderPort& exitPort, CameraProperty camera) {
    // activate and clear output render target
    outport.activateTarget();
    outport.clearTarget();

    // retrieve shader from shader property
    tgt::Shader* shader = shader_.getShader();
    if (!shader || !shader->isLinked()) {
        outport.deactivateTarget();
        return;
    }

    // activate shader and set common uniforms
    shader->activate();
    tgt::Camera cam = camera.get();
    setGlobalShaderParameters(shader, &cam);

    // bind entry and exit params and pass texture units to the shader
    TextureUnit entryUnit, entryDepthUnit, exitUnit, exitDepthUnit;
    entryPort.bindTextures(entryUnit, entryDepthUnit);
    shader->setUniform("entryPoints_", entryUnit.getUnitNumber());
    shader->setUniform("entryPointsDepth_", entryDepthUnit.getUnitNumber());
    entryPort.setTextureParameters(shader, "entryParameters_");

    exitPort.bindTextures(exitUnit, exitDepthUnit);
    shader->setUniform("exitPoints_", exitUnit.getUnitNumber());
    shader->setUniform("exitPointsDepth_", exitDepthUnit.getUnitNumber());
    exitPort.setTextureParameters(shader, "exitParameters_");

    // bind volume texture and pass it to the shader
    std::vector<VolumeStruct> volumeTextures;
    TextureUnit volUnit;
     volumeTextures.push_back(VolumeStruct(
        volumePort_.getData(),
        &volUnit,
        "volume_",
        "volumeStruct_",
        volumePort_.getTextureClampModeProperty().getValue(),
        tgt::vec4(volumePort_.getTextureBorderIntensityProperty().get()),
        volumePort_.getTextureFilterModeProperty().getValue())
    );
    bindVolumes(shader, volumeTextures, &cam, lightPosition_.get());

    // bind transfer function and pass it to the shader
    TextureUnit transferUnit;
    if (transferFunc_.get()) {
        transferUnit.activate();
        transferFunc_.get()->bind();
        transferFunc_.get()->setUniform(shader, "transferFunc_", "transferFuncTex_", transferUnit.getUnitNumber());
    }

    // render screen aligned quad
    renderQuad();

    // clean up
    shader->deactivate();
    outport.deactivateTarget();
    TextureUnit::setZeroUnit();
    LGL_ERROR;
}

void StereoRaycaster::rebuildShader() {
    shader_.setHeader(generateHeader());
    shader_.rebuild();
}

std::string StereoRaycaster::generateHeader() {
    std::string header = VolumeRaycaster::generateHeader();

    header += transferFunc_.get()->getShaderDefines();
    return header;
}

} // namespace voreen
