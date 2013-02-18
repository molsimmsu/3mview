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

#ifndef VRN_MANIPULATIONBASE_H
#define VRN_MANIPULATIONBASE_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/optionproperty.h"

namespace voreen {

/**
 * Segments the incoming volume. This is a simple
 * VolumeProcessor for demonstrating the concept.
 */
class ManipulationBase : public CachingVolumeProcessor {
public:
    ManipulationBase();
    virtual ~ManipulationBase();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "ManipulationBase";       }
    virtual std::string getCategory() const  { return "Segmentation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }

    virtual bool usesExpensiveComputation() const { return true; }
    
protected:
    virtual void setDescriptions() {
        setDescription("Base processor for segmentation");
    }

    virtual void process();

private:
    void forceUpdate();
    void segmentVolume();  
  
    VolumePort inport_;
    VolumePort outport_;
    
	StringOptionProperty manipulationType_;
    BoolProperty invertDirection_;
	FloatProperty manipulationSlider_;
    
    bool forceUpdate_;
};

} // namespace

#endif // VRN_MANIPULATIONBASE_H
