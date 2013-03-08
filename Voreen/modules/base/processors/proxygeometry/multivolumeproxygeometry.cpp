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

#include "multivolumeproxygeometry.h"

namespace voreen {

MultiVolumeProxyGeometry::MultiVolumeProxyGeometry()
    : Processor()
    , inport_(Port::INPORT, "volumehandle.volumehandle", "Volume Input", true)
    , outport_(Port::OUTPORT, "proxygeometry.geometry", "Proxy Geometry Output")
{
    addPort(inport_);
    addPort(outport_);
}

MultiVolumeProxyGeometry::~MultiVolumeProxyGeometry() {
}

Processor* MultiVolumeProxyGeometry::create() const {
    return new MultiVolumeProxyGeometry();
}

void MultiVolumeProxyGeometry::process() {
    tgtAssert(inport_.getData()->getRepresentation<VolumeRAM>(), "no volume");

    MeshListGeometry* geometry = new MeshListGeometry();

    std::vector<const VolumeBase*> data = inport_.getAllData();
    for(size_t d=0; d<data.size(); ++d) {
        if(!data[d])
            continue;

        const VolumeBase* volume = data[d];

        /*tgt::vec3 coordLlf = volume->getLLF();
        tgt::vec3 coordUrb = volume->getURB();


        MeshGeometry mesh = MeshGeometry::createCube(coordLlf, coordUrb, coordLlf, coordUrb);
        //apply dataset transformation matrix:
        mesh.transform(volume->getPhysicalToWorldMatrix());

        //reset tex coords to coords after transformation:
        for(size_t j=0; j<mesh.getFaceCount(); ++j) {
            FaceGeometry& fg = mesh.getFace(j);
            for(size_t k=0; k<fg.getVertexCount(); ++k) {
                VertexGeometry& vg = fg.getVertex(k);
                vg.setTexCoords(vg.getCoords());
            }
        }*/
	MeshGeometry mesh = createVolumeGeometry(volume);
        geometry->addMesh(mesh);
    }
    tgt::Bounds bb = geometry->getBoundingBox();
    
    MeshListGeometry* cube = new MeshListGeometry();
    cube->addMesh(MeshGeometry::createCube(bb.getLLF(), bb.getURB()));

    outport_.setData(cube);
}

MeshGeometry MultiVolumeProxyGeometry::createVolumeGeometry(const VolumeBase* inputVolume)
{
    tgt::vec3 volumeSize = inputVolume->getCubeSize();
    tgt::ivec3 numSlices = inputVolume->getDimensions();

    // vertex and tex coords of bounding box without clipping
    tgt::vec3 coordLlf = inputVolume->getLLF();
    tgt::vec3 coordUrb = inputVolume->getURB();
    const tgt::vec3 noClippingTexLlf(0, 0, 0);
    const tgt::vec3 noClippingTexUrb(1, 1, 1);

    tgt::vec3 texLlf;
    tgt::vec3 texUrb;

    texLlf = noClippingTexLlf;
    texUrb = noClippingTexUrb;

    // create output mesh
    MeshGeometry geometry = MeshGeometry::createCube(coordLlf, coordUrb, texLlf, texUrb, texLlf, texUrb);
    geometry.transform(inputVolume->getPhysicalToWorldMatrix());
    
    return geometry;
}

} // namespace
