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

#include "primitivegeometrysource.h"
#include "../utils/primitivegeometrybuilder.h"
#include "../datastructures/polyline.h"

#include "voreen/core/voreenapplication.h"
#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include "voreen/core/datastructures/geometry/pointlistgeometry.h"
#include "voreen/core/datastructures/geometry/pointsegmentlistgeometry.h"

#include "voreen/core/properties/callmemberaction.h"

#include <vector>
#include <fstream>
#include <sstream>

using tgt::vec3;
using tgt::ivec3;
using tgt::ivec2;
using std::vector;

namespace voreen {

const std::string PrimitiveGeometrySource::loggerCat_("voreen.sample.PrimitiveGeometrySource");

PrimitiveGeometrySource::PrimitiveGeometrySource()
  : Processor(),
    geometryType_("geometryType", "Geometry Type"),
    inputFile_("inputFile", "Input file", "Load OBJ", VoreenApplication::app()->getUserDataPath(), "*.obj"),
    loadGeometry_("loadGeometry", "Load Geometry"),
    clearGeometry_("clearGeometry", "Clear Geometry"),
    polylineTangent_("polylineTangent", "Polyline Tangent", 1.f, 0.f, 10.f),
    outport_(Port::OUTPORT, "geometry.pointlist", "PointList Output")
{
    geometryType_.addOption("axes", "Axes");
    geometryType_.addOption("plane", "Plane");
    geometryType_.addOption("polyline", "PolyLine");
    geometryType_.addOption("loadFile", "Load from file");

    loadGeometry_.onChange(CallMemberAction<PrimitiveGeometrySource>(this, &PrimitiveGeometrySource::readGeometry));
    clearGeometry_.onChange(CallMemberAction<PrimitiveGeometrySource>(this, &PrimitiveGeometrySource::clearGeometry));
    geometryType_.onChange(CallMemberAction<PrimitiveGeometrySource>(this, &PrimitiveGeometrySource::updatePropertyVisibility));

    
    addProperty(geometryType_);
    addProperty(inputFile_);
    addProperty(loadGeometry_);
    addProperty(clearGeometry_);
    addProperty(polylineTangent_);

    addPort(outport_);
}

Processor* PrimitiveGeometrySource::create() const {
    return new PrimitiveGeometrySource();
}

void PrimitiveGeometrySource::process() {}

void PrimitiveGeometrySource::initialize() throw (tgt::Exception) {
    Processor::initialize();

    updatePropertyVisibility();
    readGeometry();
}

void PrimitiveGeometrySource::readGeometry() {
    if (geometryType_.isSelected("axes")) {
        try {
            MeshListGeometry* geometry = new MeshListGeometry();
            
            MeshGeometry X = PrimitiveGeometryBuilder::createCylinder(
                tgt::vec3(0,0,0), tgt::vec3(10,0,0), 1, 8, tgt::vec3(1,0,0));
            MeshGeometry Y = PrimitiveGeometryBuilder::createCylinder(
                tgt::vec3(0,0,0), tgt::vec3(0,10,0), 1, 8, tgt::vec3(0,1,0));
            MeshGeometry Z = PrimitiveGeometryBuilder::createCylinder(
                tgt::vec3(0,0,0), tgt::vec3(0,0,10), 1, 8, tgt::vec3(0,0,1));
                
            geometry->addMesh(X);
            geometry->addMesh(Y);
            geometry->addMesh(Z);
            
            tgtAssert(geometry, "null pointer returned (exception expected)");
            outport_.setData(geometry);
        }
        catch (VoreenException& e) {
            LERROR(e.what());
        }
    }
    else if (geometryType_.isSelected("plane")) {
        try {
            Geometry* geometry = createPlaneGeometry();
            tgtAssert(geometry, "null pointer returned (exception expected)");
            outport_.setData(geometry);
        }
        catch (VoreenException& e) {
            LERROR(e.what());
        }
    }
    else if (geometryType_.isSelected("polyline")) {
        try {
            PolyLine line;
            line.addVertex(tgt::vec3(0, 0, 0));
            line.addVertex(tgt::vec3(1, 1, 1));
            line.addVertex(tgt::vec3(2, 2, 4));
            line.addVertex(tgt::vec3(4, 4, 4));
            
            PolyLine* bezier = line.interpolateBezier(10, polylineTangent_.get());
            Geometry* geometry = PrimitiveGeometryBuilder::createPolyLine(bezier, .05f, 8, tgt::vec3(1.f, 1.f, 0.f));
            outport_.setData(geometry);
        }
        catch (VoreenException& e) {
            LERROR(e.what());
        }
    }
    else if (geometryType_.isSelected("loadFile")) {
        try {
            Geometry* geometry = loadGeometryFromFile(inputFile_.get());
            tgtAssert(geometry, "null pointer returned (exception expected)");
            outport_.setData(geometry);
        }
        catch (VoreenException& e) {
            LERROR(e.what());
        }
    }
    else {
        LWARNING("Unknown geometry type: " << geometryType_.get());
    }

    updatePropertyVisibility();
}

Geometry* PrimitiveGeometrySource::createPlaneGeometry() const
    throw (VoreenException)
{
    FaceGeometry* geometry = new FaceGeometry();
    geometry->addVertex(VertexGeometry(tgt::vec3( 1,  1,  0)));
    geometry->addVertex(VertexGeometry(tgt::vec3(-1,  1,  0)));
    geometry->addVertex(VertexGeometry(tgt::vec3(-1, -1,  0)));
    geometry->addVertex(VertexGeometry(tgt::vec3( 1, -1,  0)));
    
    for (size_t i = 0; i < 4; i++) {
		geometry->getVertex(i).setColor(tgt::vec3( 0, 1,  1));
		geometry->getVertex(i).setNormal(tgt::vec3(0, 0, 1));
    }
    return geometry;
}

Geometry* PrimitiveGeometrySource::createCubeGeometry() const
    throw (VoreenException)
{
    MeshGeometry* geometry = new MeshGeometry();
    geometry->createCube();
    return geometry;
}

Geometry* PrimitiveGeometrySource::loadGeometryFromFile(const std::string& filename)
    throw (VoreenException)
{
    std::ifstream stream;
    stream.open(filename.c_str(), std::ios_base::in);
    if (stream.fail())
        throw VoreenException("Failed to open file for reading: " + filename);

	MeshGeometry* mesh = new MeshGeometry();
	
	std::vector<tgt::vec3> vertices;
	std::vector<tgt::vec3> vertexNormals;	
	
	readVertices(stream, vertices);
	readVertexNormals(stream, vertexNormals);
	
	bool normalsValid = false;
	
	if (vertexNormals.size() == vertices.size())
		normalsValid = true;
	
	std::vector<int> faceVertices;
	
	while (readFace(stream, faceVertices)) {
		FaceGeometry face;
		
		for (size_t i = 0; i < faceVertices.size(); i++) {
			int vertexIndex = faceVertices[i];

			VertexGeometry vertex(vertices[vertexIndex]);
			if (normalsValid)
				vertex.setNormal(vertexNormals[vertexIndex]);
				
			vertex.setColor(tgt::vec3(0.f, 1.f, 1.f));
			
			face.addVertex(vertex);
		}
		
		mesh->addFace(face);
    }
    
    return mesh;
}

void PrimitiveGeometrySource::readVertices(std::istream& stream, std::vector<tgt::vec3>& vertices) {
	char buf[80];
	
	while (stream.getline(buf, 80)) {
		if (buf[0] != 'v') return;
		
		std::stringstream string(buf + 1);
		
		float x, y, z;
		string >> x >> y >> z;
		
		vertices.push_back(tgt::vec3(x, y, z));
	}
}

void PrimitiveGeometrySource::readVertexNormals(std::istream& stream, std::vector<tgt::vec3>& vertexNormals) {
	
}

bool PrimitiveGeometrySource::readFace(std::istream& stream, std::vector<int>& faceVertices) {
	faceVertices.clear();
	
	char buf[80];
	
	if (stream.getline(buf, 80) && buf[0] == 'f') {
		std::stringstream string(buf + 1);
		
		while (1) {
			float vertexIndex = 0;
			string >> vertexIndex;
			
			if (vertexIndex > 0) {
				faceVertices.push_back(vertexIndex - 1);
			}
			else break;
		}
	
		return true;
	}

	return false;
}

void PrimitiveGeometrySource::clearGeometry() {
    outport_.setData(0);
    updatePropertyVisibility();
}

void PrimitiveGeometrySource::updatePropertyVisibility() {
    clearGeometry_.setWidgetsEnabled(outport_.getData());
}

} // namespace voreen
