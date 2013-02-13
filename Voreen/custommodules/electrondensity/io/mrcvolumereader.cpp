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

#include "mrcvolumereader.h"

#include "voreen/core/datastructures/volume/volumedecorator.h"
#include "voreen/core/io/textfilereader.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/utils/stringutils.h"

#include <fstream>
#include <iostream>
#include <assert.h>
#include <cmath>

#define PI 3.141592

#include "tgt/exception.h"
#include "tgt/vector.h"
#include "tgt/texturemanager.h"

using std::string;
using tgt::vec3;
using tgt::ivec3;
using tgt::Texture;

namespace voreen {

const std::string MRCVolumeReader::loggerCat_ = "voreen.electrondensity.MRCVolumeReader";

MRCVolumeReader::MRCVolumeReader(ProgressBar* progress) : VolumeReader(progress)
{
    extensions_.push_back("mrc");
    extensions_.push_back("ccp4");
}

VolumeCollection* MRCVolumeReader::read(const std::string &url)
    throw (tgt::FileException, tgt::IOException, std::bad_alloc)
{
    VolumeCollection* volumeCollection = new VolumeCollection();

    VolumeURL origin(url);
    std::string fileName = origin.getPath();
    LINFO(fileName);
    
    std::ifstream mrc;
    mrc.open(fileName.c_str(), std::ios::binary);
    if (!mrc.is_open()) {
        LWARNING("Can't open stream");
    }
    else {
        int dim[3]; // grid dimensions i.e. numbers of voxels for each dimension
        mrc.read((char*)(&dim), sizeof(dim));
        std::cout << "X: " << dim[0] << std::endl; // number of columns (fastest changing in map)
        std::cout << "Y: " << dim[1] << std::endl; // number of rows
        std::cout << "Z: " << dim[2] << std::endl; // number of sections (slowest changing in map)
        
        int numVoxels = dim[0] * dim[1] * dim[2]; // total number of voxels in volume
        std::cout << "numVoxels: " << numVoxels << std::endl;
        
        int dataType; // see below
        mrc.read((char*)(&dataType), sizeof(dataType));
        std::cout << "dataType: " << dataType << std::endl;
        
        int dataSize = 0; // i.e. 8-bit, 16-bit or 32-bit
             if (dataType == 0) dataSize = 1; // signed 8-bit bytes range -128 to 127
        else if (dataType == 1) dataSize = 2; // 16-bit halfwords
        else if (dataType == 2) dataSize = 4; // 32-bit reals
        else if (dataType == 6) dataSize = 2; // unsigned 16-bit range 0 to 65535
        
        tgtAssert(dataSize, "Datasize is 0 at MRCVolumeReader::read()");
        
        int totalDataSize = dataSize * numVoxels;
        
        int start[3]; // numbers of first columns i.e. offset of the volume origin in voxel coordinates
        mrc.read((char*)(&start), sizeof(start));
        std::cout << "startX: " << start[0] << std::endl; // number of columns (fastest changing in map)
        std::cout << "startY: " << start[1] << std::endl; // number of rows
        std::cout << "startZ: " << start[2] << std::endl; // number of sections (slowest changing in map)

        int gridSize[3];
        mrc.read((char*)(&gridSize), sizeof(gridSize));
        std::cout << "gridSizeX: " << gridSize[0] << std::endl;
        std::cout << "gridSizeY: " << gridSize[1] << std::endl; 
        std::cout << "gridSizeZ: " << gridSize[2] << std::endl;
     
        float cellDimensions[3]; // cell dimensions in angstroms
        mrc.read((char*)(&cellDimensions), sizeof(cellDimensions));
        std::cout << "cellX: " << cellDimensions[0] << std::endl; 
        std::cout << "cellY: " << cellDimensions[1] << std::endl; 
        std::cout << "cellZ: " << cellDimensions[2] << std::endl; 
        
        float scale[3]; // pixelSpacing i.e. scale from voxel to real-word coordinates
        scale[0] = cellDimensions[0] / gridSize[0];
        scale[1] = cellDimensions[1] / gridSize[1];
        scale[2] = cellDimensions[2] / gridSize[2];
        std::cout << "pixelSpacingX: " << scale[0] << std::endl; 
        std::cout << "pixelSpacingY: " << scale[1] << std::endl; 
        std::cout << "pixelSpacingZ: " << scale[2] << std::endl; 
        
        float angles[3]; // cell angles in degrees
        mrc.read((char*)(&angles), sizeof(angles));
        std::cout << "cellAngleX: " << angles[0] << std::endl;
        std::cout << "cellAngleY: " << angles[1] << std::endl;
        std::cout << "cellAngleZ: " << angles[2] << std::endl;
        
        int axes[3]; // Which axis corresponds to columns, rows and sections (1,2,3 for X,Y,Z)
        mrc.read((char*)(&axes), sizeof(axes));
        std::cout << "axesX: " << axes[0] << std::endl;
        std::cout << "axesY: " << axes[1] << std::endl; 
        std::cout << "axesZ: " << axes[2] << std::endl; 
        
        float origin[3];
        mrc.seekg(4*49, std::ios::beg);
        mrc.read((char*)(&origin), sizeof(origin));
        std::cout << "originX: " << origin[0] << std::endl; 
        std::cout << "originY: " << origin[1] << std::endl; 
        std::cout << "originZ: " << origin[2] << std::endl; 
        
        void* data = malloc(totalDataSize);
        mrc.seekg(1024, std::ios::beg);
        mrc.read((char*)data, totalDataSize);
        mrc.close();
        
        VolumeRAM* targetDataset;
        
        /**/ if (dataType == 0) {
            targetDataset = new VolumeAtomic<int8_t>(ivec3(dim[a], dim[b], dim[c]));
            fillVolume<int8_t>(targetDataset, data, dim, axes);
        }
        else if (dataType == 1) {
            targetDataset = new VolumeAtomic<int16_t>(ivec3(dim[a], dim[b], dim[c]));
            fillVolume<int16_t>(targetDataset, data, dim, axes);
        }
        else if (dataType == 2) {
            targetDataset = new VolumeAtomic<float>(ivec3(dim[a], dim[b], dim[c]));
            fillVolume<float>(targetDataset, data, dim, axes);
        }
        else if (dataType == 6) {
            targetDataset = new VolumeAtomic<uint16_t>(ivec3(dim[a], dim[b], dim[c]));
            fillVolume<uint16_t>(targetDataset, data, dim, axes);
        }
        else LERROR("Unsupported data type at MRCVolumeReader::read()");
            
        free(data);
        
        angles[0] *= (PI / 180.);
        angles[1] *= (PI / 180.);
        angles[2] *= (PI / 180.);
        
        float row[3][3];
        
        // X
        row[0][0] = 1;
        row[0][1] = 0;
        row[0][2] = 0;
        
        // Y
        row[1][0] = cos(angles[2]); // cos(gamma)
        row[1][1] = sin(angles[2]); // sin(gamma)
        row[1][2] = 0;
        
        // Z
        row[2][0] = cos(angles[1]); // cos(beta)
        row[2][1] = (cos(angles[0]) - row[2][0] * row[1][0]) / row[1][1];  // [cos(alpha) - cos(beta)*cos(gamma)] / sin(gamma)
        row[2][2] = sqrt(1 - row[2][0] * row[2][0] - row[2][1] * row[2][1]); // squared length is 1
        
        tgt::Matrix4<float> transform
        (
            row[0][0], row[1][0], row[2][0], 0,
            row[0][1], row[1][1], row[2][1], 0,
            row[0][2], row[1][2], row[2][2], 0,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        
        int a = axes[0]-1;
        int b = axes[1]-1;
        int c = axes[2]-1;
        
        Volume* volumeHandle = new Volume(
            targetDataset,                                                 // data
            vec3(scale[a], scale[b], scale[c]),                            // scale
            vec3(start[a]*scale[a], start[b]*scale[b], start[c]*scale[c]), // offset
            transform                                                      // transform
        );
        
        volumeCollection->add(volumeHandle);
    }
    
    if (!volumeCollection->empty())
        volumeCollection->first()->setOrigin(VolumeURL(fileName));
        
    return volumeCollection;
}

VolumeReader* MRCVolumeReader::create(ProgressBar* progress) const {
    return new MRCVolumeReader(progress);
}

} // namespace voreen
