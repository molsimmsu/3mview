/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2012 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#include "sampleprocessor.h"
#include "tgt/textureunit.h"

namespace voreen {

SampleProcessor::SampleProcessor()
    : RenderProcessor()
    , inport_(Port::INPORT, "inport")
    , outport_(Port::OUTPORT, "outport")
    , saturation_("saturation", "Saturation")
{
    // register ports and properties
    addPort(inport_);
    addPort(outport_);
    addProperty(saturation_);
}

Processor* SampleProcessor::create() const {
    return new SampleProcessor();
}

void SampleProcessor::process() {
    // activate and clear output render target
    outport_.activateTarget();
    outport_.clearTarget();

    // bind input image to texture units
    tgt::TextureUnit colorUnit, depthUnit;
    inport_.bindTextures(colorUnit.getEnum(), depthUnit.getEnum());

    // activate shader and pass data
    shader_->activate();
    setGlobalShaderParameters(shader_);

    // pass input image to shader
    inport_.setTextureParameters(shader_, "textureParameters_");
    shader_->setUniform("colorTex_", colorUnit.getUnitNumber());
    shader_->setUniform("depthTex_", depthUnit.getUnitNumber());

    // pass property value to shader
    shader_->setUniform("saturation_", saturation_.get());

    // render screen aligned quad to run the fragment shader
    renderQuad();

    // cleanup
    shader_->deactivate();
    outport_.deactivateTarget();
    tgt::TextureUnit::setZeroUnit();

    // check for OpenGL errors
    LGL_ERROR;
}

void SampleProcessor::initialize() throw (VoreenException) {
    // call superclass function first
    RenderProcessor::initialize();

    // load fragment shader 'sample.frag' 
    shader_ = ShdrMgr.loadSeparate("passthrough.vert", "sample.frag", generateHeader(), false);
}

void SampleProcessor::deinitialize() throw (VoreenException) {
    // free shader
    ShdrMgr.dispose(shader_);
    shader_ = 0;

    // call superclass function last
    RenderProcessor::deinitialize();
}

} // namespace
