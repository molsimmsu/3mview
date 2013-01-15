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

#ifndef VRN_VOLUMEOPERATORSPLIT_H
#define VRN_VOLUMEOPERATORSPLIT_H

#include "voreen/core/datastructures/volume/operators/volumeoperatorminmax.h"
#include "voreen/core/datastructures/volume/volumeoperator.h"

namespace voreen {

// Base class, defines interface for the operator (-> apply):
class VolumeOperatorSplitBase : public UnaryVolumeOperatorBase {
public:
    virtual Volume* apply(const VolumeBase* volume, ProgressBar* progressBar = 0) const = 0;
};

// Generic implementation:
template<typename T>
class VolumeOperatorSplitGeneric : public VolumeOperatorSplitBase {
public:
    virtual Volume* apply(const VolumeBase* volume, ProgressBar* progressBar = 0) const;
    //Implement isCompatible using a handy macro:
    IS_COMPATIBLE
};

template<typename T>
Volume* VolumeOperatorSplitGeneric<T>::apply(const VolumeBase* vh, ProgressBar* progressBar) const {
    const VolumeRAM* v = vh->getRepresentation<VolumeRAM>();
    if(!v)
        return 0;

    const VolumeAtomic<T>* va = dynamic_cast<const VolumeAtomic<T>*>(v);
    if (!va)
        return 0;

    VolumeAtomic<T>* out = va->clone();

    T max = VolumeOperatorMaxValue::apply(va);
    T min = VolumeOperatorMinValue::apply(va);
    
    VRN_FOR_EACH_VOXEL_WITH_PROGRESS(index, tgt::svec3(0, 0, 0), out->getDimensions(), progressBar) {
	//if (out->voxel(index) >= 0.5 * (max + min)) 
	  out->voxel(index) = 0;
	//else out->voxel(index) = 1;
    }
    if (progressBar)
        progressBar->setProgress(1.f);

    return new Volume(out, vh);
}

typedef UniversalUnaryVolumeOperatorGeneric<VolumeOperatorSplitBase> VolumeOperatorSplit;

} // namespace

#endif // VRN_VOLUMEOPERATORSPLIT_H
