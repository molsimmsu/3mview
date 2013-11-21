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

#ifndef VRN_DXVOLUMEREADER_H
#define VRN_DXVOLUMEREADER_H

#include "voreen/core/io/volumereader.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"

namespace voreen {

class IOProgress;

/**
 * Reads a multi-image TIFF file into a volume dataset.
 */
class DXVolumeReader : public VolumeReader {
public:
    DXVolumeReader(ProgressBar* progress = 0);
    ~DXVolumeReader() {}
    virtual VolumeReader* create(ProgressBar* progress = 0) const;

    virtual std::string getClassName() const   { return "DXVolumeReader"; }
    virtual std::string getFormatDescription() const { return "MRC/CCP4 electron density map format"; }

    virtual VolumeCollection* read(const std::string& url)
        throw (tgt::FileException, tgt::IOException, std::bad_alloc);

private:


    template<typename T>
    T* ptr(T* ptr, int x, int y, int z, int sizeX, int sizeY) {
        return ptr + x + y*sizeX + z*sizeX*sizeY;
    }
    
    static const std::string loggerCat_;
};

} // namespace voreen

#endif // VRN_DXVolumeReader_H
