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

#ifndef VRN_STEREOCANVASRENDERER_H
#define VRN_STEREOCANVASRENDERER_H

#include "modules/core/processors/output/canvasrenderer.h"


namespace tgt {
class GLCanvas;  // forward declaration
class Texture;
}

namespace voreen {

/**
 * A StereoCanvasRenderer is a terminating element in a network. Its purpose is to copy
 * its input to the associated canvas and to issue canvas updates on invalidations.
 * Additionally, the StereoCanvasRenderer can take snapshots of arbitrary dimensions.
 */
class VRN_CORE_API StereoCanvasRenderer : public CanvasRenderer {
public:
    StereoCanvasRenderer();
    ~StereoCanvasRenderer();

    virtual std::string getClassName() const    { return "StereoCanvas"; }
    virtual std::string getCategory() const     { return "Output"; }
    virtual CodeState getCodeState() const      { return CODE_STATE_STABLE; }
    
    virtual Processor* create() const;

};

} // namespace voreen

#endif // VRN_STEREOCANVASRENDERER_H
