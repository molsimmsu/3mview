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

#include "segmentationbase.h"
#include "voreen/core/datastructures/volume/volumeram.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumeminmax.h"
#include "operators/volumeoperatorsplit.h"

namespace voreen {

SegmentationBase::SegmentationBase()
    : CachingVolumeProcessor()
    , inport_(Port::INPORT, "volumehandle.inport", "Volume Input")
    , outport_(Port::OUTPORT, "volumehandle.outport", "Segmentation Output", false)
    , segmentCount_("segmentCount", "Segment count", 3, 1, 10)
    , enableProcessing_("enableProcessing", "Enable")
    , forceUpdate_(true)
{
    // register ports and properties
    addPort(inport_);
    addPort(outport_);
    
    enableProcessing_.onChange(CallMemberAction<SegmentationBase>(this, &SegmentationBase::forceUpdate));
    addProperty(enableProcessing_);
    addProperty(segmentCount_);
}

SegmentationBase::~SegmentationBase() {
}

Processor* SegmentationBase::create() const {
    return new SegmentationBase();
}

void SegmentationBase::process() {
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
void SegmentationBase::forceUpdate() {
    forceUpdate_ = true;
}

void SegmentationBase::segmentVolume() {
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