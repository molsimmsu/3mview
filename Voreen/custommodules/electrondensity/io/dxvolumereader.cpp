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

#include "dxvolumereader.h"
#include "../datastructures/moleculevolume.h"

#include "voreen/core/datastructures/volume/volumedecorator.h"
#include "voreen/core/io/textfilereader.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/utils/stringutils.h"

#include <fstream>
#include <iostream>
#include <assert.h>
#include <cmath>

#define PI 3.141592
#define LINELENGTH 2040

#include "tgt/exception.h"
#include "tgt/vector.h"
#include "tgt/texturemanager.h"

using std::string;
using tgt::vec3;
using tgt::ivec3;
using tgt::Texture;

namespace voreen {

const std::string DXVolumeReader::loggerCat_ = "voreen.electrondensity.DXVolumeReader";

DXVolumeReader::DXVolumeReader(ProgressBar* progress) : VolumeReader(progress)
{
    extensions_.push_back("dx");
}

VolumeCollection* DXVolumeReader::read(const std::string &url)
    throw (tgt::FileException, tgt::IOException, std::bad_alloc)
{
    VolumeCollection* volumeCollection = new VolumeCollection();

    VolumeURL origin(url);
    std::string fileName = origin.getPath();
    LINFO(fileName);
    
    std::ifstream dx;
    dx.open(fileName.c_str());
    if (!dx.is_open()) {
        LWARNING("Can't open stream");
    }
    else {
    
        char inbuffer[LINELENGTH];
        int dim[3]; // grid dimensions i.e. numbers of voxels for each dimension
        float origin[3]; float xscale[3]; float yscale[3]; float zscale[3]; float tempscale[3];
        bool isBinary=false;
        //skip comments
        do{dx.getline(inbuffer,LINELENGTH);
        }while (inbuffer[0] == '#');

        /* get the number of grid points */
        if (std::sscanf(inbuffer, "object 1 class gridpositions counts %d %d %d", dim, dim+1, dim+2) !=3){
            LWARNING("Can't read grid dimentions");
            return NULL;
        };
        std::cout << "X: " << dim[0] << std::endl; // number of columns (fastest changing in map)
        std::cout << "Y: " << dim[1] << std::endl; // number of rows
        std::cout << "Z: " << dim[2] << std::endl; // number of sections (slowest changing in map)
        int numVoxels = dim[0] * dim[1] * dim[2]; // total number of voxels in volume
        std::cout << "numVoxels: " << numVoxels << std::endl;

        /* get the cell origin */
        dx.getline(inbuffer,LINELENGTH);
        std::cout << inbuffer << std::endl; 
        if (std::sscanf(inbuffer, "origin %e %e %e", origin, origin+1, origin+2) != 3) {
            LWARNING("Error reading grid origin.");
            std::cout << "originX: " << origin[0] << std::endl; 
            std::cout << "originY: " << origin[1] << std::endl; 
            std::cout << "originZ: " << origin[2] << std::endl; 
            return NULL;
        }
        std::cout << "originX: " << origin[0] << std::endl; 
        std::cout << "originY: " << origin[1] << std::endl; 
        std::cout << "originZ: " << origin[2] << std::endl;        
        
        /* get the cell dimensions, axis may be any vector from i element ro i+1
        no messing with angles*/
        dx.getline(inbuffer,LINELENGTH);
        if (std::sscanf(inbuffer, "delta %e %e %e", xscale, xscale+1, xscale+2) != 3) {
            LWARNING("Error reading cell x-scaling.");
            return NULL;
        }
        dx.getline(inbuffer,LINELENGTH);
        if (std::sscanf(inbuffer, "delta %e %e %e", yscale, yscale+1, yscale+2) != 3) {
            LWARNING("Error reading cell y-scaling.");
            return NULL;
        }
        dx.getline(inbuffer,LINELENGTH);
        if (std::sscanf(inbuffer, "delta %e %e %e", zscale, zscale+1, zscale+2) != 3) {
            LWARNING("Error reading cell z-scaling.");
            return NULL;
        }
        tempscale[0]=xscale[0];
        tempscale[1]=yscale[1];
        tempscale[2]=zscale[2];
        
        /*Skip one line because it doe not contain usable info*/
        dx.getline(inbuffer,LINELENGTH);
        
        /* Lets find out if the info is binary or not  */
        dx.getline(inbuffer,LINELENGTH);
        if (std::strstr(inbuffer, "binary")) {
            isBinary = true;
        }
        
        int dataSize = 4; // 32-bit reals
        int totalDataSize = dataSize * numVoxels;
                
        int axes[3]={1, 2, 3};

/*


        void* data = malloc(totalDataSize);



        mrc.seekg(1024, std::ios::beg);
        mrc.read((char*)data, totalDataSize);
        mrc.close();
*/

        
        VolumeRAM* targetDataset;
        
        
        int a = axes[0]-1;
        int b = axes[1]-1;
        int c = axes[2]-1;
        
        targetDataset = new VolumeAtomic<float>(ivec3(dim[a], dim[b], dim[c]));

        float rawVoxel;

        int i, j, k, line, count, sout;
        i=j=k=line=0;
        char *p;
        for (count = 0; count < numVoxels;) {
            ++line;
            dx.getline(inbuffer,LINELENGTH);
            p=inbuffer;

            // chop line into whitespace separated tokens and parse them one by one.
            while (*p != '\n' && *p != '\0') 
            {
 
                // skip over whitespace and try to parse non-blank as number
                while (*p != '\0' && (*p == ' ' || *p == '\t' || *p == '\n')) ++p;
                sout = sscanf(p, "%e", &rawVoxel);
                if (sout < 0) break; // end of line/string. get a new one.
           
                // a 0 return value means non-parsable as number.
                if (*p == 0) 
                {   LWARNING("unparsable input");
                    //return NULL;
                }
             
                // success! add to dataset.
                if (sout == 1) {
                    ++count;
                    targetDataset->setVoxelNormalized(rawVoxel, ivec3(i, j, k));
                    //targetDataset =VolumeAtomic<float>::voxel(ivec3(i, j, k))=rawVoxel;
                    k++;
                    if (k >= dim[c]) {
                        k = 0; j++;
                            if (j >= dim[b]) {
                             j = 0; i++;
                        }
                    }
                }
             
                   // skip over the parsed text and search for next blank.
                   while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n') ++p;
            }
        }



        
        
        //targetDataset->VolumeAtomic<float>::voxel(ivec3(i, j, k))=value

        //fillVolume<float>(targetDataset, data, dim, axes);
          
        //free(data);
     
        
        tgt::Matrix4<float> transform
        (
            1.0f, 0.0f, 0.0f, 0,
            0.0f, 1.0f, 0.0f, 0,
            0.0f, 0.0f, 1.0f, 0,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        
        Volume* volumeHandle = new MoleculeVolume(
            targetDataset,                                      // data
            vec3(tempscale[a], tempscale[b], tempscale[c]),     // scale
            vec3(origin[a], origin[b], origin[c]),              // offset
            transform                                           // transform
        );
        
        volumeCollection->add(volumeHandle);
    }
    
    if (!volumeCollection->empty())
        volumeCollection->first()->setOrigin(VolumeURL(fileName));
        
    return volumeCollection;
}

VolumeReader* DXVolumeReader::create(ProgressBar* progress) const {
    return new DXVolumeReader(progress);
}

} // namespace voreen
