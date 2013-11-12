/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2013 University of Muenster, Germany.                        *
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

#ifndef VRN_VOLUMESOURCEPROCESSOR_H
#define VRN_VOLUMESOURCEPROCESSOR_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/volumeport.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/properties/volumeurlproperty.h"
#include "voreen/core/properties/volumeinfoproperty.h"

namespace voreen {

class Volume;

/**
 * Volume data set supplier in the network.
 */
class VRN_CORE_API EmptyVolumeSource : public Processor {

public:
    EmptyVolumeSource();
    virtual Processor* create() const;

    virtual std::string getClassName() const    { return "EmptyVolumeSource";    }
    virtual std::string getCategory() const     { return "Input";           }
    virtual CodeState getCodeState() const      { return CODE_STATE_STABLE; }

protected:
    virtual void setDescriptions() {
        setDescription("Provides a single volume.");
    }

    virtual void process() {}

    /// The volume port the empty data set is written to.
    VolumePort outport_;

    static const std::string loggerCat_;
};

} // namespace

#endif
