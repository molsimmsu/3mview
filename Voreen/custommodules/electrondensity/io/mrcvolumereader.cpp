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

#include "voreen/core/datastructures/volume/volumeatomic.h"
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
        int dim[3];
        mrc.read((char*)(&dim), sizeof(dim));
        std::cout << "X: " << dim[0] << std::endl; // number of columns (fastest changing in map)
        std::cout << "Y: " << dim[1] << std::endl; // number of rows
        std::cout << "Z: " << dim[2] << std::endl; // number of sections (slowest changing in map)
        
        int numVoxels = dim[0] * dim[1] * dim[2];
        std::cout << "numVoxels: " << numVoxels << std::endl;
        
        int dataType;
        mrc.read((char*)(&dataType), sizeof(dataType));
        std::cout << "dataType: " << dataType << std::endl;
        
        int dataSize = 0;
             if (dataType == 0) dataSize = 1; // signed 8-bit bytes range -128 to 127
        else if (dataType == 1) dataSize = 2; // 16-bit halfwords
        else if (dataType == 2) dataSize = 4; // 32-bit reals
        else if (dataType == 6) dataSize = 2; // unsigned 16-bit range 0 to 65535
        
        tgtAssert(dataSize, "Datasize is 0 at MRCVolumeReader::read()");
        
        int totalDataSize = dataSize * numVoxels;
        
        int start[3]; // numbers of first columns
        
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
        
        float scale[3];
        scale[0] = cellDimensions[0] / gridSize[0];
        scale[1] = cellDimensions[1] / gridSize[1];
        scale[2] = cellDimensions[2] / gridSize[2];
        std::cout << "pixelSpacingX: " << scale[0] << std::endl; 
        std::cout << "pixelSpacingY: " << scale[1] << std::endl; 
        std::cout << "pixelSpacingZ: " << scale[2] << std::endl; 
        
        float cellAngles[3]; // cell angles in degrees

        mrc.read((char*)(&cellAngles), sizeof(cellAngles));
        std::cout << "cellAngleX: " << cellAngles[0] << std::endl;
        std::cout << "cellAngleY: " << cellAngles[1] << std::endl;
        std::cout << "cellAngleZ: " << cellAngles[2] << std::endl;
        
        int axes[3]; 
        
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
        
        int a = axes[0]-1;
        int b = axes[1]-1;
        int c = axes[2]-1;
        
        VolumeRAM* targetDataset = new VolumeRAM_Float(ivec3(dim[a], dim[b], dim[c]));
        
        std::cout << "Created volume with sizeX=" << dim[a] << " sizeY=" << dim[b] << " sizeZ=" << dim[c] << std::endl;
        
        size_t i[3];
        for (i[2] = 0; i[2] < dim[c]; i[2]++)
        for (i[1] = 0; i[1] < dim[b]; i[1]++)
        for (i[0] = 0; i[0] < dim[a]; i[0]++)
        {
            ((VolumeRAM_Float*)targetDataset)->voxel(i[0],i[1],i[2]) = *ptr((float*)data, i[a], i[b], i[c], dim[b], dim[a]);
        }
            
        free(data);
        
        float row[3][3];
        
        // X
        row[0][0] = 1;
        row[0][1] = 0;
        row[0][2] = 0;
        
        // Y
        row[1][0] = cos(cellAngles[2] * PI / 180.);
        row[1][1] = sin(cellAngles[2] * PI / 180.);
        row[1][2] = 0;
        
        // Z
        row[2][0] = 0;
        row[2][1] = 0;
        row[2][2] = 1;
        
        tgt::Matrix4<float> transform
        (
            row[0][0], row[1][0], row[2][0], 0,
            row[0][1], row[1][1], row[2][1], 0,
            row[0][2], row[1][2], row[2][2], 0,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        
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
