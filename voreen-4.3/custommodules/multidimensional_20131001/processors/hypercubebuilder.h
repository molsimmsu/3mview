/***********************************************************************************
* *
* Voreen - The Volume Rendering Engine *
* *
* Copyright (C) 2005-2012 University of Muenster, Germany. *
* Visualization and Computer Graphics Group <http://viscg.uni-muenster.de> *
* For a list of authors please refer to the file "CREDITS.txt". *
* *
* This file is part of the Voreen software package. Voreen is free software: *
* you can redistribute it and/or modify it under the terms of the GNU General *
* Public License version 2 as published by the Free Software Foundation. *
* *
* Voreen is distributed in the hope that it will be useful, but WITHOUT ANY *
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR *
* A PARTICULAR PURPOSE. See the GNU General Public License for more details. *
* *
* You should have received a copy of the GNU General Public License in the file *
* "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
* *
* For non-commercial academic use see the license exception specified in the file *
* "LICENSE-academic.txt". To get information about commercial licensing please *
* contact the authors. *
* *
***********************************************************************************/

#ifndef VRN_HYPERCUBEBUILDER_H
#define VRN_HYPERCUBEBUILDER_H

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/geometryport.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/matrixproperty.h"
using namespace voreen;

#include "tgt/vector.h"
#include <Eigen/Core>
using namespace Eigen;

#include "../datastructures/multidimensionalspace.h"

class HypercubeBuilder : public Processor {
public:
    HypercubeBuilder();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "HypercubeBuilder"; }
    virtual std::string getCategory() const { return "Input"; }
    virtual CodeState getCodeState() const { return CODE_STATE_STABLE; }

protected:
    virtual void setDescriptions() {
        setDescription("Allows to create and manipulate a hypercube");
    }

    virtual void process() {}

private:
    void rebuildProjections();
    void rotateBasis();
    void buildGeometry();
    void randomRotation();
    void printBasis();
    
    tgt::vec3 pointToVec3(const MatrixXd points, size_t j);
    
    IntProperty numDimensions_;
    IntProperty numProjectionDimensions_;
    FloatProperty vertexSize_;
    FloatProperty distance_;
    BoolProperty drawQuasiEdges_;
    FloatProperty quasiEdgeDistance_;
    BoolProperty drawEdges_;
    FloatProperty edgeSize_;
    IntProperty axis1_;
    IntProperty axis2_;
    IntProperty rotate_;
    IntProperty numTiles_;
    ButtonProperty buildGeometry_;
    ButtonProperty randomRotation_;
    ButtonProperty printBasis_;
    StringOptionProperty geometryType_;
    
    Space* space;
    ProjectionSubspace* projection;
    
    MatrixXf basis3D;

    GeometryPort outport_;

    static const std::string loggerCat_;
};

#endif // VRN_HYPERCUBEBUILDER_H
