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

#include "camerasplitter.h"
#include "tgt/textureunit.h"

using tgt::vec3;

namespace voreen {

CameraSplitter::CameraSplitter()
    : RenderProcessor()
    , inport_(Port::INPORT, "inport")
    , outport_(Port::OUTPORT, "outport")
    , eyeSeparation_("eyeSeparation", "Eye Separation")
    , mainCamera_("mainCamera", "Main Camera")
    , leftCamera_("leftCamera", "Left Camera")
    , rightCamera_("rightCamera", "Right Camera")
{
    // register ports and properties
    addPort(inport_);
    addPort(outport_);
    
    addProperty(eyeSeparation_);
    addProperty(mainCamera_);
    addProperty(leftCamera_);
    addProperty(rightCamera_);
}

Processor* CameraSplitter::create() const {
    return new CameraSplitter();
}

void CameraSplitter::process() {
	outport_.setRenderTarget(inport_.getRenderTarget());

	leftCamera_.set(mainCamera_.get());
	rightCamera_.set(mainCamera_.get());
}

} // namespace
