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

#include "moleculecollection.h"

#include "molecule.h"

using std::vector;

namespace voreen {

const std::string MoleculeCollection::loggerCat_ = "voreen.MoleculeCollection";

MoleculeCollection::MoleculeCollection() :
    Observable<MoleculeCollectionObserver>()
{
}

MoleculeCollection::~MoleculeCollection()     {
    clear();
}

void MoleculeCollection::add(VolumeBase* volumeHandle) {

    tgtAssert(volumeHandle, "Null pointer as Volume passed");
    if (!contains(volumeHandle)) {
        volumeHandles_.push_back(volumeHandle);
        volumeHandle->addObserver(this);
        notifyVolumeAdded(volumeHandle);
    }
}

void MoleculeCollection::add(const MoleculeCollection* volumeCollection) {

    tgtAssert(volumeCollection, "Unexpected null pointer");
    for (size_t i=0; i<volumeCollection->size(); ++i) {
        add(volumeCollection->at(i));
    }
}

void MoleculeCollection::remove(const VolumeBase* volumeHandle){
    std::vector<VolumeBase*>::iterator handleIter = find(volumeHandle);
    if (handleIter != volumeHandles_.end()) {
        volumeHandles_.erase(handleIter);
        notifyVolumeRemoved(volumeHandle);
    }
}

void MoleculeCollection::remove(const MoleculeCollection* volumeCollection) {
   tgtAssert(volumeCollection, "Unexpected null pointer");
    for (size_t i=0; i<volumeCollection->size(); ++i) {
        remove(volumeCollection->at(i));
    }
}

std::vector<VolumeBase*>::iterator MoleculeCollection::find(const VolumeBase* volumeHandle) {
    return std::find(volumeHandles_.begin(), volumeHandles_.end(), volumeHandle);
}

std::vector<VolumeBase*>::const_iterator MoleculeCollection::find(const VolumeBase* volumeHandle) const {
    return std::find(volumeHandles_.begin(), volumeHandles_.end(), volumeHandle);
}

bool MoleculeCollection::contains(const VolumeBase* volumeHandle) const {
    return (find(volumeHandle) != volumeHandles_.end());
}

VolumeBase* MoleculeCollection::at(size_t i) const {
    tgtAssert(i < volumeHandles_.size(), "Invalid index");
    return volumeHandles_.at(i);
}

VolumeBase* MoleculeCollection::first() const {
    return (!empty() ? volumeHandles_.front() : 0);
}

void MoleculeCollection::clear() {
    while (!empty())
        remove(first());
}

MoleculeCollection* MoleculeCollection::selectEntity(const std::string& /*name*/) const {
    return new MoleculeCollection();
}

MoleculeCollection* MoleculeCollection::selectRepresentation(const std::string& /*name*/) const {
    return new MoleculeCollection();
}

MoleculeCollection* MoleculeCollection::selectModality(const Modality& modality) const {

    MoleculeCollection* collection = new MoleculeCollection();
    for (size_t i=0; i<volumeHandles_.size(); ++i) {
        if (volumeHandles_[i]->getModality() == modality)
            collection->add(volumeHandles_[i]);
    }
    return collection;
}

voreen::MoleculeCollection* MoleculeCollection::selectOrigin(const VolumeURL& origin) const {
    MoleculeCollection* collection = new MoleculeCollection();
    for (size_t i=0; i<volumeHandles_.size(); ++i) {
        Volume* vh = dynamic_cast<Volume*>(volumeHandles_[i]);
        if (vh && vh->getOrigin() == origin)
            collection->add(volumeHandles_[i]);
    }
    return collection;
}

MoleculeCollection* MoleculeCollection::subCollection(size_t start, size_t end) const {
    MoleculeCollection* subCollection = new MoleculeCollection();
    tgtAssert(start <= end, "invalid indices");
    tgtAssert(start < volumeHandles_.size(), "invalid start index");
    tgtAssert(end < volumeHandles_.size(), "invalid end index");
    for (size_t index = start; index <= end; index++)
        subCollection->add(volumeHandles_.at(index));
    return subCollection;
}

MoleculeCollection* MoleculeCollection::subCollection(const std::vector<size_t>& indices) const {
    MoleculeCollection* subCollection = new MoleculeCollection();
    for (size_t i=0; i<indices.size(); i++) {
        tgtAssert(indices.at(i) < volumeHandles_.size(), "invalid index");
        subCollection->add(volumeHandles_.at(indices.at(i)));
    }
    return subCollection;
}


size_t MoleculeCollection::size() const {
    return volumeHandles_.size();
}

bool MoleculeCollection::empty() const {
    return (size() == 0);
}

void MoleculeCollection::notifyVolumeAdded(const VolumeBase* handle) {
    const vector<MoleculeCollectionObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->volumeAdded(this, handle);

}

void MoleculeCollection::notifyVolumeRemoved(const VolumeBase* handle) {
    const vector<MoleculeCollectionObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->volumeRemoved(this, handle);
}

void MoleculeCollection::notifyVolumeChanged(const VolumeBase* handle) {
    const vector<MoleculeCollectionObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->volumeChanged(this, handle);
}

// implementation of VolumeHandleObserver interface
void MoleculeCollection::volumeChange(const VolumeBase* handle) {
    if (contains(handle))
        notifyVolumeChanged(handle);

}

// implementation of VolumeHandleObserver interface
void MoleculeCollection::volumeHandleDelete(const VolumeBase* handle) {
    if (contains(handle))
        remove(handle);
}

} // namespace
