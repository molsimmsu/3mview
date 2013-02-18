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

#include "volumecollectionraycaster.h"

#include "tgt/textureunit.h"
#include "voreen/core/ports/conditions/portconditionvolumetype.h"

#include <sstream>

using tgt::vec3;
using tgt::TextureUnit;

namespace voreen {

VolumeCollectionRaycaster::VolumeCollectionRaycaster()
    : VolumeRaycaster()
    , volumeInport1_(Port::INPORT, "volume1", "Volume1 Input", false, Processor::INVALID_PROGRAM)
    , volumeInport2_(Port::INPORT, "volume2", "Volume2 Input", false, Processor::INVALID_PROGRAM)
    , volumeInport3_(Port::INPORT, "volume3", "Volume3 Input", false, Processor::INVALID_PROGRAM)
    , volumeInport4_(Port::INPORT, "volume4", "Volume4 Input", false, Processor::INVALID_PROGRAM)
    , entryPort_(Port::INPORT, "image.entrypoints", "Entry-points Input", false, Processor::INVALID_PROGRAM, RenderPort::RENDERSIZE_ORIGIN)
    , exitPort_(Port::INPORT, "image.exitpoints", "Exit-points Input", false, Processor::INVALID_PROGRAM, RenderPort::RENDERSIZE_ORIGIN)
    , outport_(Port::OUTPORT, "image.output", "Image Output", true, Processor::INVALID_PROGRAM, RenderPort::RENDERSIZE_RECEIVER, GL_RGBA16F_ARB)
    , outport1_(Port::OUTPORT, "image.output1", "Image1 Output", true, Processor::INVALID_PROGRAM, RenderPort::RENDERSIZE_RECEIVER, GL_RGBA16F_ARB)
    , outport2_(Port::OUTPORT, "image.output2", "Image2 Output", true, Processor::INVALID_PROGRAM, RenderPort::RENDERSIZE_RECEIVER, GL_RGBA16F_ARB)
    , shaderProp_("raycast.prg", "Raycasting Shader", "rc_multivolume.frag", "passthrough.vert")
    , shadeMode1_("shading1", "Shading 1", Processor::INVALID_PROGRAM)
    , shadeMode2_("shading2", "Shading 2", Processor::INVALID_PROGRAM)
    , shadeMode3_("shading3", "Shading 3", Processor::INVALID_PROGRAM)
    , shadeMode4_("shading4", "Shading 4", Processor::INVALID_PROGRAM)
    , transferFunc1_("transferFunction1", "Transfer Function 1")
    , transferFunc2_("transferFunction2", "Transfer Function 2")
    , transferFunc3_("transferFunction3", "Transfer Function 3")
    , transferFunc4_("transferFunction4", "Transfer Function 4")
    , camera_("camera", "Camera", tgt::Camera(vec3(0.f, 0.f, 3.5f), vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f)))
    , compositingMode1_("compositing1", "Compositing (OP2)", Processor::INVALID_PROGRAM)
    , compositingMode2_("compositing2", "Compositing (OP3)", Processor::INVALID_PROGRAM)
{
    // ports
    volumeInport1_.addCondition(new PortConditionVolumeTypeGL());
    volumeInport2_.addCondition(new PortConditionVolumeTypeGL());
    volumeInport3_.addCondition(new PortConditionVolumeTypeGL());
    volumeInport4_.addCondition(new PortConditionVolumeTypeGL());
    volumeInport1_.showTextureAccessProperties(true);
    volumeInport2_.showTextureAccessProperties(true);
    volumeInport3_.showTextureAccessProperties(true);
    volumeInport4_.showTextureAccessProperties(true);
    addPort(volumeInport1_);
    addPort(volumeInport2_);
    addPort(volumeInport3_);
    addPort(volumeInport4_);
    addPort(entryPort_);
    addPort(exitPort_);
    addPort(outport_);
    addPort(outport1_);
    addPort(outport2_);

    // shader property
    addProperty(shaderProp_);

    // tf properties
    addProperty(transferFunc1_);
    addProperty(transferFunc2_);
    addProperty(transferFunc3_);
    addProperty(transferFunc4_);
    addProperty(camera_);

    // shading properties
    addProperty(gradientMode_);
    shadeMode1_.addOption("none", "none");
    shadeMode1_.addOption("phong-diffuse", "Phong (Diffuse)");
    shadeMode1_.addOption("phong-specular", "Phong (Specular)");
    shadeMode1_.addOption("phong-diffuse-ambient", "Phong (Diffuse+Amb.)");
    shadeMode1_.addOption("phong-diffuse-specular", "Phong (Diffuse+Spec.)");
    shadeMode1_.addOption("phong", "Phong (Full)");
    shadeMode1_.addOption("toon", "Toon");
    shadeMode1_.select("phong");
    addProperty(shadeMode1_);
    shadeMode2_.addOption("none", "none");
    shadeMode2_.addOption("phong-diffuse", "Phong (Diffuse)");
    shadeMode2_.addOption("phong-specular", "Phong (Specular)");
    shadeMode2_.addOption("phong-diffuse-ambient", "Phong (Diffuse+Amb.)");
    shadeMode2_.addOption("phong-diffuse-specular", "Phong (Diffuse+Spec.)");
    shadeMode2_.addOption("phong", "Phong (Full)");
    shadeMode2_.addOption("toon", "Toon");
    shadeMode2_.select("phong");
    addProperty(shadeMode2_);
    shadeMode3_.addOption("none", "none");
    shadeMode3_.addOption("phong-diffuse", "Phong (Diffuse)");
    shadeMode3_.addOption("phong-specular", "Phong (Specular)");
    shadeMode3_.addOption("phong-diffuse-ambient", "Phong (Diffuse+Amb.)");
    shadeMode3_.addOption("phong-diffuse-specular", "Phong (Diffuse+Spec.)");
    shadeMode3_.addOption("phong", "Phong (Full)");
    shadeMode3_.addOption("toon", "Toon");
    shadeMode3_.select("phong");
    addProperty(shadeMode3_);
    shadeMode4_.addOption("none", "none");
    shadeMode4_.addOption("phong-diffuse", "Phong (Diffuse)");
    shadeMode4_.addOption("phong-specular", "Phong (Specular)");
    shadeMode4_.addOption("phong-diffuse-ambient", "Phong (Diffuse+Amb.)");
    shadeMode4_.addOption("phong-diffuse-specular", "Phong (Diffuse+Spec.)");
    shadeMode4_.addOption("phong", "Phong (Full)");
    shadeMode4_.addOption("toon", "Toon");
    shadeMode4_.select("phong");
    addProperty(shadeMode4_);

    // compositing modes
    addProperty(compositingMode_);
    compositingMode1_.addOption("dvr", "DVR");
    compositingMode1_.addOption("mip", "MIP");
    compositingMode1_.addOption("iso", "ISO");
    compositingMode1_.addOption("fhp", "W-FHP");
    //compositingMode1_.addOption("fhn", "FHN");
    addProperty(compositingMode1_);
    compositingMode2_.addOption("dvr", "DVR");
    compositingMode2_.addOption("mip", "MIP");
    compositingMode2_.addOption("iso", "ISO");
    compositingMode2_.addOption("fhp", "W-FHP");
    //compositingMode2_.addOption("fhn", "FHN");
    addProperty(compositingMode2_);
    addProperty(isoValue_);

    // lighting properties
    addProperty(lightPosition_);
    addProperty(lightAmbient_);
    addProperty(lightDiffuse_);
    addProperty(lightSpecular_);
    addProperty(materialShininess_);
    addProperty(applyLightAttenuation_);
    addProperty(lightAttenuation_);

    // assign lighting properties to property group
    lightPosition_.setGroupID("lighting");
    lightAmbient_.setGroupID("lighting");
    lightDiffuse_.setGroupID("lighting");
    lightSpecular_.setGroupID("lighting");
    materialShininess_.setGroupID("lighting");
    applyLightAttenuation_.setGroupID("lighting");
    lightAttenuation_.setGroupID("lighting");
    setPropertyGroupGuiName("lighting", "Lighting Parameters");

    // listen to changes of properties that influence the GUI state (i.e. visibility of other props)
    classificationMode_.onChange(CallMemberAction<VolumeCollectionRaycaster>(this, &VolumeCollectionRaycaster::adjustPropertyVisibilities));
    shadeMode_.onChange(CallMemberAction<VolumeCollectionRaycaster>(this, &VolumeCollectionRaycaster::adjustPropertyVisibilities));
    compositingMode_.onChange(CallMemberAction<VolumeCollectionRaycaster>(this, &VolumeCollectionRaycaster::adjustPropertyVisibilities));
    compositingMode1_.onChange(CallMemberAction<VolumeCollectionRaycaster>(this, &VolumeCollectionRaycaster::adjustPropertyVisibilities));
    compositingMode2_.onChange(CallMemberAction<VolumeCollectionRaycaster>(this, &VolumeCollectionRaycaster::adjustPropertyVisibilities));
    applyLightAttenuation_.onChange(CallMemberAction<VolumeCollectionRaycaster>(this, &VolumeCollectionRaycaster::adjustPropertyVisibilities));
}

Processor* VolumeCollectionRaycaster::create() const {
    return new VolumeCollectionRaycaster();
}

void VolumeCollectionRaycaster::initialize() throw (tgt::Exception) {
    VolumeRaycaster::initialize();
    compile();

    portGroup_.initialize();
    portGroup_.addPort(outport_);
    portGroup_.addPort(outport1_);
    portGroup_.addPort(outport2_);
    portGroup_.deactivateTargets();

    adjustPropertyVisibilities();
}

void VolumeCollectionRaycaster::deinitialize() throw (tgt::Exception) {
    portGroup_.deinitialize();
    VolumeRaycaster::deinitialize();
}

void VolumeCollectionRaycaster::compile() {
    shaderProp_.setHeader(generateHeader());
    shaderProp_.rebuild();
}

bool VolumeCollectionRaycaster::isReady() const {
    //check if all inports are connected:
    if(!entryPort_.isReady() || !exitPort_.isReady() || !volumeInport1_.isReady())
        return false;

    //check if at least one outport is connected:
    if (!outport_.isReady() && !outport1_.isReady() && !outport2_.isReady())
        return false;

    return true;
}

void VolumeCollectionRaycaster::process() {
    // compile program if needed
    if (getInvalidationLevel() >= Processor::INVALID_PROGRAM)
        compile();
    LGL_ERROR;

    // bind transfer function
    TextureUnit transferUnit1, transferUnit2, transferUnit3, transferUnit4;
    transferUnit1.activate();
    if (transferFunc1_.get())
        transferFunc1_.get()->bind();

    transferUnit2.activate();
    if (transferFunc2_.get())
        transferFunc2_.get()->bind();

    transferUnit3.activate();
    if (transferFunc3_.get())
        transferFunc3_.get()->bind();

    transferUnit4.activate();
    if (transferFunc4_.get())
        transferFunc4_.get()->bind();

    portGroup_.activateTargets();
    portGroup_.clearTargets();
    LGL_ERROR;

    transferFunc1_.setVolumeHandle(volumeInport1_.getData());
    transferFunc2_.setVolumeHandle(volumeInport2_.getData());
    transferFunc3_.setVolumeHandle(volumeInport3_.getData());
    transferFunc4_.setVolumeHandle(volumeInport4_.getData());

    TextureUnit entryUnit, entryDepthUnit, exitUnit, exitDepthUnit;
    // bind entry params
    entryPort_.bindTextures(entryUnit.getEnum(), entryDepthUnit.getEnum());
    LGL_ERROR;

    // bind exit params
    exitPort_.bindTextures(exitUnit.getEnum(), exitDepthUnit.getEnum());
    LGL_ERROR;

    // vector containing the volumes to bind; is passed to bindVolumes()
    std::vector<VolumeStruct> volumeTextures;
    std::vector<const VolumeBase*> volumeHandles;

    // bind volumes
    TextureUnit volUnit1, volUnit2, volUnit3, volUnit4;
    if (volumeInport1_.isReady()) {
        volumeTextures.push_back(VolumeStruct(
                    volumeInport1_.getData(),
                    &volUnit1,
                    "volume1_","volumeStruct1_",
                    volumeInport1_.getTextureClampModeProperty().getValue(),
                    tgt::vec4(volumeInport1_.getTextureBorderIntensityProperty().get()),
                    volumeInport1_.getTextureFilterModeProperty().getValue())
                );
        volumeHandles.push_back(volumeInport1_.getData());
    }
    if (volumeInport2_.isReady()) {
        volumeTextures.push_back(VolumeStruct(
                    volumeInport2_.getData(),
                    &volUnit2,
                    "volume2_","volumeStruct2_",
                    volumeInport2_.getTextureClampModeProperty().getValue(),
                    tgt::vec4(volumeInport2_.getTextureBorderIntensityProperty().get()),
                    volumeInport2_.getTextureFilterModeProperty().getValue())
                );
        volumeHandles.push_back(volumeInport2_.getData());
    }
    if (volumeInport3_.isReady()) {
        volumeTextures.push_back(VolumeStruct(
                    volumeInport3_.getData(),
                    &volUnit3,
                    "volume3_","volumeStruct3_",
                    volumeInport3_.getTextureClampModeProperty().getValue(),
                    tgt::vec4(volumeInport3_.getTextureBorderIntensityProperty().get()),
                    volumeInport3_.getTextureFilterModeProperty().getValue())
                );
        volumeHandles.push_back(volumeInport3_.getData());
    }
    if (volumeInport4_.isReady()) {
        volumeTextures.push_back(VolumeStruct(
                    volumeInport4_.getData(),
                    &volUnit4,
                    "volume4_","volumeStruct4_",
                    volumeInport4_.getTextureClampModeProperty().getValue(),
                    tgt::vec4(volumeInport4_.getTextureBorderIntensityProperty().get()),
                    volumeInport4_.getTextureFilterModeProperty().getValue())
                );
        volumeHandles.push_back(volumeInport4_.getData());
    }

    // initialize shader
    tgt::Shader* raycastPrg = shaderProp_.getShader();
    raycastPrg->activate();

    // set common uniforms used by all shaders
    tgt::Camera cam = camera_.get();
    setGlobalShaderParameters(raycastPrg, &cam);
    // bind the volumes and pass the necessary information to the shader
    bindVolumes(raycastPrg, volumeTextures, &cam, lightPosition_.get());

    // pass the remaining uniforms to the shader
    raycastPrg->setUniform("entryPoints_", entryUnit.getUnitNumber());
    raycastPrg->setUniform("entryPointsDepth_", entryDepthUnit.getUnitNumber());
    entryPort_.setTextureParameters(raycastPrg, "entryParameters_");
    raycastPrg->setUniform("exitPoints_", exitUnit.getUnitNumber());
    raycastPrg->setUniform("exitPointsDepth_", exitDepthUnit.getUnitNumber());
    exitPort_.setTextureParameters(raycastPrg, "exitParameters_");

    if (compositingMode_.get() ==  "iso" ||
        compositingMode1_.get() == "iso" ||
        compositingMode2_.get() == "iso")
        raycastPrg->setUniform("isoValue_", isoValue_.get());

    if(volumeInport1_.isReady())
        transferFunc1_.get()->setUniform(raycastPrg, "transferFunc1_", "transferFuncTex1_", transferUnit1.getUnitNumber());
    if(volumeInport2_.isReady())
        transferFunc2_.get()->setUniform(raycastPrg, "transferFunc2_", "transferFuncTex2_", transferUnit2.getUnitNumber());
    if(volumeInport3_.isReady())
        transferFunc3_.get()->setUniform(raycastPrg, "transferFunc3_", "transferFuncTex3_", transferUnit3.getUnitNumber());
    if(volumeInport4_.isReady())
        transferFunc4_.get()->setUniform(raycastPrg, "transferFunc4_", "transferFuncTex4_", transferUnit4.getUnitNumber());

    // determine ray step length in world coords
    if (volumeTextures.size() > 0) {
        float voxelSizeWorld = 999.f;
        float voxelSizeTexture = 999.f;
        for(size_t i=0; i<volumeHandles.size(); ++i) {
            const VolumeBase* volume = volumeHandles[i];
            tgtAssert(volume, "No volume");
            tgt::ivec3 volDim = volume->getDimensions();
            tgt::vec3 cubeSizeWorld = volume->getCubeSize() * volume->getPhysicalToWorldMatrix().getScalingPart();

            float tVoxelSizeWorld = tgt::max(cubeSizeWorld / tgt::vec3(volDim));
            if (tVoxelSizeWorld < voxelSizeWorld) {
                voxelSizeWorld = tVoxelSizeWorld;
                voxelSizeTexture = tgt::max(1.f / tgt::vec3(volDim));
            }
        }

        float samplingStepSizeWorld = voxelSizeWorld / samplingRate_.get();
        float samplingStepSizeTexture = voxelSizeTexture / samplingRate_.get();

        if (interactionMode()) {
            samplingStepSizeWorld /= interactionQuality_.get();
            samplingStepSizeTexture /= interactionQuality_.get();
        }

        raycastPrg->setUniform("samplingStepSize_", samplingStepSizeWorld);
        if (compositingMode_.isSelected("dvr")  ||
            (compositingMode1_.isSelected("dvr") && outport1_.isConnected()) ||
            (compositingMode2_.isSelected("dvr") && outport2_.isConnected()) ) {
            // adapts the compositing of the multivolume RC to the one of the singlevolume RC (see below).
            raycastPrg->setUniform("mvOpacityCorrectionFactor_", samplingStepSizeTexture / samplingStepSizeWorld);
        }
        LGL_ERROR;
    }
    LGL_ERROR;

    renderQuad();

    raycastPrg->deactivate();
    portGroup_.deactivateTargets();

    glActiveTexture(GL_TEXTURE0);
    LGL_ERROR;
}

std::string VolumeCollectionRaycaster::generateHeader() {
    std::string headerSource = VolumeRaycaster::generateHeader();

    if(volumeInport1_.isReady())
        headerSource += "#define VOLUME_1_ACTIVE\n";
    if(volumeInport2_.isReady())
        headerSource += "#define VOLUME_2_ACTIVE\n";
    if(volumeInport3_.isReady())
        headerSource += "#define VOLUME_3_ACTIVE\n";
    if(volumeInport4_.isReady())
        headerSource += "#define VOLUME_4_ACTIVE\n";

    headerSource += "#define TF_SAMPLER_TYPE_1 " + transferFunc1_.get()->getSamplerType() + "\n";
    headerSource += "#define TF_SAMPLER_TYPE_2 " + transferFunc2_.get()->getSamplerType() + "\n";
    headerSource += "#define TF_SAMPLER_TYPE_3 " + transferFunc3_.get()->getSamplerType() + "\n";
    headerSource += "#define TF_SAMPLER_TYPE_4 " + transferFunc4_.get()->getSamplerType() + "\n";

    // configure shading mode
    headerSource += "#define APPLY_SHADING_1(n, pos, lPos, cPos, ka, kd, ks) ";
    if (shadeMode1_.isSelected("none"))
        headerSource += "ka;\n";
    else if (shadeMode1_.isSelected("phong-diffuse"))
        headerSource += "phongShadingD(n, pos, lPos, cPos, kd);\n";
    else if (shadeMode1_.isSelected("phong-specular"))
        headerSource += "phongShadingS(n, pos, lPos, cPos, ks);\n";
    else if (shadeMode1_.isSelected("phong-diffuse-ambient"))
        headerSource += "phongShadingDA(n, pos, lPos, cPos, kd, ka);\n";
    else if (shadeMode1_.isSelected("phong-diffuse-specular"))
        headerSource += "phongShadingDS(n, pos, lPos, cPos, kd, ks);\n";
    else if (shadeMode1_.isSelected("phong"))
        headerSource += "phongShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode1_.isSelected("toon"))
        headerSource += "toonShading(n, pos, lPos, cPos, kd, 3);\n";
    else if (shadeMode1_.isSelected("cook-torrance"))
        headerSource += "cookTorranceShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode1_.isSelected("oren-nayar"))
        headerSource += "orenNayarShading(n, pos, lPos, cPos, ka, kd);\n";
    else if (shadeMode1_.isSelected("lafortune"))
        headerSource += "lafortuneShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode1_.isSelected("ward"))
        headerSource += "wardShading(n, pos, lPos, cPos, ka, kd, ks);\n";

    headerSource += "#define APPLY_SHADING_2(n, pos, lPos, cPos, ka, kd, ks) ";
    if (shadeMode2_.isSelected("none"))
        headerSource += "ka;\n";
    else if (shadeMode2_.isSelected("phong-diffuse"))
        headerSource += "phongShadingD(n, pos, lPos, cPos, kd);\n";
    else if (shadeMode2_.isSelected("phong-specular"))
        headerSource += "phongShadingS(n, pos, lPos, cPos, ks);\n";
    else if (shadeMode2_.isSelected("phong-diffuse-ambient"))
        headerSource += "phongShadingDA(n, pos, lPos, cPos, kd, ka);\n";
    else if (shadeMode2_.isSelected("phong-diffuse-specular"))
        headerSource += "phongShadingDS(n, pos, lPos, cPos, kd, ks);\n";
    else if (shadeMode2_.isSelected("phong"))
        headerSource += "phongShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode2_.isSelected("toon"))
        headerSource += "toonShading(n, pos, lPos, cPos, kd, 3);\n";
    else if (shadeMode2_.isSelected("cook-torrance"))
        headerSource += "cookTorranceShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode2_.isSelected("oren-nayar"))
        headerSource += "orenNayarShading(n, pos, lPos, cPos, ka, kd);\n";
    else if (shadeMode2_.isSelected("lafortune"))
        headerSource += "lafortuneShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode2_.isSelected("ward"))
        headerSource += "wardShading(n, pos, lPos, cPos, ka, kd, ks);\n";

    headerSource += "#define APPLY_SHADING_3(n, pos, lPos, cPos, ka, kd, ks) ";
    if (shadeMode3_.isSelected("none"))
        headerSource += "ka;\n";
    else if (shadeMode3_.isSelected("phong-diffuse"))
        headerSource += "phongShadingD(n, pos, lPos, cPos, kd);\n";
    else if (shadeMode3_.isSelected("phong-specular"))
        headerSource += "phongShadingS(n, pos, lPos, cPos, ks);\n";
    else if (shadeMode3_.isSelected("phong-diffuse-ambient"))
        headerSource += "phongShadingDA(n, pos, lPos, cPos, kd, ka);\n";
    else if (shadeMode3_.isSelected("phong-diffuse-specular"))
        headerSource += "phongShadingDS(n, pos, lPos, cPos, kd, ks);\n";
    else if (shadeMode3_.isSelected("phong"))
        headerSource += "phongShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode3_.isSelected("toon"))
        headerSource += "toonShading(n, pos, lPos, cPos, kd, 3);\n";
    else if (shadeMode3_.isSelected("cook-torrance"))
        headerSource += "cookTorranceShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode3_.isSelected("oren-nayar"))
        headerSource += "orenNayarShading(n, pos, lPos, cPos, ka, kd);\n";
    else if (shadeMode3_.isSelected("lafortune"))
        headerSource += "lafortuneShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode3_.isSelected("ward"))
        headerSource += "wardShading(n, pos, lPos, cPos, ka, kd, ks);\n";

    headerSource += "#define APPLY_SHADING_4(n, pos, lPos, cPos, ka, kd, ks) ";
    if (shadeMode4_.isSelected("none"))
        headerSource += "ka;\n";
    else if (shadeMode4_.isSelected("phong-diffuse"))
        headerSource += "phongShadingD(n, pos, lPos, cPos, kd);\n";
    else if (shadeMode4_.isSelected("phong-specular"))
        headerSource += "phongShadingS(n, pos, lPos, cPos, ks);\n";
    else if (shadeMode4_.isSelected("phong-diffuse-ambient"))
        headerSource += "phongShadingDA(n, pos, lPos, cPos, kd, ka);\n";
    else if (shadeMode4_.isSelected("phong-diffuse-specular"))
        headerSource += "phongShadingDS(n, pos, lPos, cPos, kd, ks);\n";
    else if (shadeMode4_.isSelected("phong"))
        headerSource += "phongShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode4_.isSelected("toon"))
        headerSource += "toonShading(n, pos, lPos, cPos, kd, 3);\n";
    else if (shadeMode4_.isSelected("cook-torrance"))
        headerSource += "cookTorranceShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode4_.isSelected("oren-nayar"))
        headerSource += "orenNayarShading(n, pos, lPos, cPos, ka, kd);\n";
    else if (shadeMode4_.isSelected("lafortune"))
        headerSource += "lafortuneShading(n, pos, lPos, cPos, ka, kd, ks);\n";
    else if (shadeMode4_.isSelected("ward"))
        headerSource += "wardShading(n, pos, lPos, cPos, ka, kd, ks);\n";

    // DVR opacity correction function adapting the MV compositing to the SVRC compositing,
    // used by the compositing macros below.
    // The adaption is necessary, because the multivolume RC samples in world space
    // instead of in texture space. Due to differing sampling base intervals, we would otherwise
    // still get correct compositing results, but the compositing would slightly differ from
    // the one performed by the SingleVolumeRaycaster.
    headerSource += "uniform float mvOpacityCorrectionFactor_;\n";
    headerSource += "vec4 mvOpacityCorrection(in vec4 color) {\n";
    headerSource += "  return vec4(color.rgb, 1.0 - pow(1.0-color.a, mvOpacityCorrectionFactor_));\n";
    headerSource += "}\n";

    // configure compositing mode for port 1
    headerSource += "#define RC_APPLY_COMPOSITING_1(result, color, samplePos, gradient, t, samplingStepSize, tDepth) ";
    if (compositingMode_.isSelected("dvr"))
        headerSource += "compositeDVR(result, mvOpacityCorrection(color), t, samplingStepSize, tDepth);\n";
    else if (compositingMode_.isSelected("mip"))
        headerSource += "compositeMIP(result, color, t, tDepth);\n";
    else if (compositingMode_.isSelected("iso"))
        headerSource += "compositeISO(result, color, t, tDepth, isoValue_);\n";
    else if (compositingMode_.isSelected("fhp"))
        headerSource += "compositeFHP(samplePos, result, t, tDepth);\n";
    else if (compositingMode_.isSelected("fhn"))
        headerSource += "compositeFHN(gradient, result, t, tDepth);\n";

    // configure compositing mode for port 2
    headerSource += "#define RC_APPLY_COMPOSITING_2(result, color, samplePos, gradient, t, samplingStepSize, tDepth) ";
    if (compositingMode1_.isSelected("dvr"))
        headerSource += "compositeDVR(result, mvOpacityCorrection(color), t, samplingStepSize, tDepth);\n";
    else if (compositingMode1_.isSelected("mip"))
        headerSource += "compositeMIP(result, color, t, tDepth);\n";
    else if (compositingMode1_.isSelected("iso"))
        headerSource += "compositeISO(result, color, t, tDepth, isoValue_);\n";
    else if (compositingMode1_.isSelected("fhp"))
        headerSource += "compositeFHP(samplePos, result, t, tDepth);\n";
    else if (compositingMode1_.isSelected("fhn"))
        headerSource += "compositeFHN(gradient, result, t, tDepth);\n";

    // configure compositing mode for port 3
    headerSource += "#define RC_APPLY_COMPOSITING_3(result, color, samplePos, gradient, t, samplingStepSize, tDepth) ";
    if (compositingMode2_.isSelected("dvr"))
        headerSource += "compositeDVR(result, mvOpacityCorrection(color), t, samplingStepSize, tDepth);\n";
    else if (compositingMode2_.isSelected("mip"))
        headerSource += "compositeMIP(result, color, t, tDepth);\n";
    else if (compositingMode2_.isSelected("iso"))
        headerSource += "compositeISO(result, color, t, tDepth, isoValue_);\n";
    else if (compositingMode2_.isSelected("fhp"))
        headerSource += "compositeFHP(samplePos, result, t, tDepth);\n";
    else if (compositingMode2_.isSelected("fhn"))
        headerSource += "compositeFHN(gradient, result, t, tDepth);\n";

    portGroup_.reattachTargets();
    headerSource += portGroup_.generateHeader(shaderProp_.getShader());
    return headerSource;
}

void VolumeCollectionRaycaster::adjustPropertyVisibilities() {
    bool useLighting = !shadeMode1_.isSelected("none") |
                       !shadeMode2_.isSelected("none") |
                       !shadeMode3_.isSelected("none") |
                       !shadeMode4_.isSelected("none");
    setPropertyGroupVisible("lighting", useLighting);

    bool useIsovalue = (compositingMode_.isSelected("iso")  ||
        compositingMode1_.isSelected("iso") ||
        compositingMode2_.isSelected("iso")   );
    isoValue_.setVisible(useIsovalue);

    lightAttenuation_.setVisible(applyLightAttenuation_.get());
}

} // namespace
