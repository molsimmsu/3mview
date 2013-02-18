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

#include "densitymapcollectionsource.h"

#include "voreen/core/processors/processorwidget.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"

namespace voreen {

const std::string DensityMapCollectionSource::loggerCat_("voreen.core.DensityMapCollectionSource");

DensityMapCollectionSource::DensityMapCollectionSource()
    : Processor()
    , coProcessorOutport_(Port::OUTPORT, "coProcessor", "Co-processor", true)
    , outport_(Port::OUTPORT, "volumecollection", "VolumeCollection Output", false)
    , volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
{
    addPort(coProcessorOutport_);
    addPort(outport_);
    addProperty(volumeURLList_);
}

DensityMapCollectionSource::~DensityMapCollectionSource() {
}

Processor* DensityMapCollectionSource::create() const {
    return new DensityMapCollectionSource();
}

void DensityMapCollectionSource::process() {
    // nothing
}

void DensityMapCollectionSource::initialize() throw (tgt::Exception) {
    Processor::initialize();

    volumeURLList_.loadVolumes(false, true);

    outport_.setData(volumeURLList_.getVolumes(true), true);

    if (getProcessorWidget())
        getProcessorWidget()->updateFromProcessor();
}

void DensityMapCollectionSource::invalidate(int inv) {
    outport_.setData(volumeURLList_.getVolumes(true), true);

    Processor::invalidate(inv);
}

void DensityMapCollectionSource::setVolumeCollection(VolumeCollection* collection, bool owner) {
    volumeURLList_.clear();
    if (!collection)
        return;
    for (size_t i=0; i<collection->size(); i++)
        volumeURLList_.addVolume(collection->at(i), owner, true);
}

VolumeCollection* DensityMapCollectionSource::getVolumeCollection() const {
    return volumeURLList_.getVolumes(false);
}

} // namespace
