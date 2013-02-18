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

namespace voreen {

ManipulationBase::ManipulationBase()
    : Processor()
	, manipulationType_("manipulationType", "Manipulation type")
	, invertDirection_("invertDirection", "Invert direction")
    , manipulationSlider_("manipulationSlider", "Manipulation slider", 0, -1, 1)
    , forceUpdate_(true)
{
    // register ports and properties
    //addPort(inport_);
    //addPort(outport_);
	
	manipulationType_.addOption("rotateX", "Rotate X");
	manipulationType_.addOption("moveX", "Move X");
    
    manipulationType_.onChange(CallMemberAction<ManipulationBase>(this, &ManipulationBase::manipulate));
	
    addProperty(manipulationType_);
    addProperty(invertDirection_);
    addProperty(manipulationSlider_);
}

ManipulationBase::~ManipulationBase() {
}

Processor* ManipulationBase::create() const {
    return new ManipulationBase();
}

void ManipulationBase::process() {
    if (!enableProcessing_.get()) {
        // TODO Create single-segment volume or return NULL
        outport_.setData(const_cast<VolumeBase*>(inport_.getData()), false);
    }
    else if (forceUpdate_ || inport_.hasChanged()) {
        segmentVolume();
    }
}

// private methods
//
void ManipulationBase::forceUpdate() {
    forceUpdate_ = true;
}

void ManipulationBase::segmentVolume() {
    // TODO Make this virtual and move to subclass
    
    const VolumeBase* handle = inport_.getData();
    tgtAssert(handle, "Inport has no data");

    forceUpdate_ = false;

    if (handle->getRepresentation<VolumeRAM>()) {
        const VolumeRAM* vr = handle->getRepresentation<VolumeRAM>();
	const VolumeRAM_UInt8* vac = dynamic_cast<const VolumeRAM_UInt8*>(vr);
	VolumeRAM_UInt8* va = vac->clone();
	
	//tgt::svec3 dim = vr->getDimensions();
	size_t numVoxels = va->getNumVoxels();
	float min = handle->getDerivedData<VolumeMinMax>()->getMin();
	float max = handle->getDerivedData<VolumeMinMax>()->getMax();
	float interval = (max - min) / segmentCount_.get();
	float val;
	
	for (size_t i = 0; i < numVoxels; i++) {
	  val = vr->getVoxelNormalized(i);
	  
	  uint8_t segmentIndex = (val - min) / interval;
	  va->voxel(i) = segmentIndex;
	}
	
        Volume* vol = new Volume(va, handle);
        outport_.setData(vol);
    }
    else {
        outport_.setData(0);
    }
}

} // namespace
