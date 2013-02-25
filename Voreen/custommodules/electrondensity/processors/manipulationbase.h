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

#include "tgt/timer.h"
#include "tgt/event/eventhandler.h"
using namespace tgt;

#include "voreen/core/ports/allports.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/optionproperty.h"

#include "../properties/volumeselectionproperty.h"
#include "densitymapcollectionsource.h"

namespace voreen {

class SpaceballEventListener : public EventListener {
public:
    SpaceballEventListener();
    virtual ~SpaceballEventListener() {}

    virtual void timerEvent(TimeEvent* e);
};
 
 
class ManipulationBase : public Processor {
public:
    ManipulationBase();
    virtual ~ManipulationBase();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "ManipulationBase";       }
    virtual std::string getCategory() const  { return "Manipulation";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void update(tgt::vec3 offset, tgt::vec3 matrix);
    
    virtual void invalidate(int inv = INVALID_RESULT);
    
protected:
    virtual void setDescriptions() {
        setDescription("Base processor for manipulation");
    }

    virtual void process();

private:
    DensityMapCollectionSource* getSourceProcessor() const;

    const VolumeCollection* getInputVolumeCollection() const;
    
    void updateSelection();
    
    void forceUpdate();
    
  
    CoProcessorPort inport_;
    
    VolumeSelectionProperty volumeSelection_;
	StringOptionProperty manipulationType_;
	StringOptionProperty manipulationAxis_;
    BoolProperty invertDirection_;
	FloatProperty manipulationSlider_;
	
	Timer* timer_;
	SpaceballEventListener* spaceballListener_;
	EventHandler* eventHandler_;
};

} // namespace

#endif // VRN_MANIPULATIONBASE_H
