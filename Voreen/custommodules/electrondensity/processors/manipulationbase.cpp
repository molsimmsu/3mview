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

#include "manipulationbase.h"
#include "densitymapcollectionsource.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"

namespace voreen {

ManipulationBase::ManipulationBase()
    : Processor()
    , inport_(Port::INPORT, "volumeCollection", "Volume Collection")
	, manipulationType_("manipulationType", "Manipulation type")
	, manipulationAxis_("manipulationAxis", "Manipulation axis")
	, invertDirection_("invertDirection", "Invert direction")
    , manipulationSlider_("manipulationSlider", "Manipulation slider", 0, -1, 1)
{
    // register ports and properties
    addPort(inport_);
    //addPort(outport_);
	
	manipulationType_.addOption("rotate", "Rotate");
	manipulationType_.addOption("move", "Move");
	
	manipulationAxis_.addOption("x", "X");
	manipulationAxis_.addOption("y", "Y");
	manipulationAxis_.addOption("z", "Z");
    
    manipulationSlider_.onChange(CallMemberAction<ManipulationBase>(this, &ManipulationBase::forceUpdate));
	
    addProperty(manipulationType_);
    addProperty(manipulationAxis_);
    addProperty(invertDirection_);
    addProperty(manipulationSlider_);
}

ManipulationBase::~ManipulationBase() {
}

Processor* ManipulationBase::create() const {
    return new ManipulationBase();
}

void ManipulationBase::process() {
    
}

// private methods
//
void ManipulationBase::forceUpdate() {
    Processor* processor = inport_.getConnectedProcessor();
    if (processor == 0) return;
    
    if (typeid(*processor) == typeid(DensityMapCollectionSource)) {
        const VolumeCollection* collection = static_cast<DensityMapCollectionSource*>(processor)->getVolumeCollection();
        if (collection == 0 || collection->size() == 0) return;
        
        for (size_t i = 0; i < collection->size(); i++) {
            VolumeBase* volume = collection->at(i);
            if (typeid(*volume) != typeid(Volume)) {
                LWARNING("Base class is not an instance of Volume");
                continue;
            }
            
            if (manipulationType_.get() == "move") {
                tgt::vec3 offset =  volume->getOffset();
                
                float offsetSize = 10;
                if (invertDirection_.get() == true) offsetSize = -offsetSize;
                /**/ if (manipulationAxis_.get() == "x") offset[0] += offsetSize;
                else if (manipulationAxis_.get() == "y") offset[1] += offsetSize;
                else if (manipulationAxis_.get() == "z") offset[2] += offsetSize;
                
                static_cast<Volume*>(volume)->setOffset(offset);
            }
            else if (manipulationType_.get() == "rotate") {
                tgt::mat4 transform = volume->getPhysicalToWorldMatrix();
                tgt::mat4 transformMatrix;
                
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
                
                static_cast<Volume*>(volume)->setPhysicalToWorldMatrix(transform * transformMatrix);
            }
        }
        
        processor->getPort("volumecollection")->invalidatePort();
    }
}

} // namespace
