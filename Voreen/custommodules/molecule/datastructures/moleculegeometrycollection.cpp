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

void MoleculeCollection::add(Molecule* moleculeHandle) {

    tgtAssert(moleculeHandle, "Null pointer as Molecule passed");
    if (!contains(moleculeHandle)) {
        moleculeHandles_.push_back(moleculeHandle);
        moleculeHandle->addObserver(this);
        notifyMoleculeAdded(moleculeHandle);
    }
}

void MoleculeCollection::add(const MoleculeCollection* moleculeCollection) {

    tgtAssert(moleculeCollection, "Unexpected null pointer");
    for (size_t i=0; i<moleculeCollection->size(); ++i) {
        add(moleculeCollection->at(i));
    }
}

void MoleculeCollection::remove(const Molecule* moleculeHandle){
    std::vector<Molecule*>::iterator handleIter = find(moleculeHandle);
    if (handleIter != moleculeHandles_.end()) {
        moleculeHandles_.erase(handleIter);
        notifyMoleculeRemoved(moleculeHandle);
    }
}

void MoleculeCollection::remove(const MoleculeCollection* moleculeCollection) {
   tgtAssert(moleculeCollection, "Unexpected null pointer");
    for (size_t i=0; i<moleculeCollection->size(); ++i) {
        remove(moleculeCollection->at(i));
    }
}

std::vector<Molecule*>::iterator MoleculeCollection::find(const Molecule* moleculeHandle) {
    return std::find(moleculeHandles_.begin(), moleculeHandles_.end(), moleculeHandle);
}

std::vector<Molecule*>::const_iterator MoleculeCollection::find(const Molecule* moleculeHandle) const {
    return std::find(moleculeHandles_.begin(), moleculeHandles_.end(), moleculeHandle);
}

bool MoleculeCollection::contains(const Molecule* moleculeHandle) const {
    return (find(moleculeHandle) != moleculeHandles_.end());
}

Molecule* MoleculeCollection::at(size_t i) const {
    tgtAssert(i < moleculeHandles_.size(), "Invalid index");
    return moleculeHandles_.at(i);
}

Molecule* MoleculeCollection::first() const {
    return (!empty() ? moleculeHandles_.front() : 0);
}

void MoleculeCollection::clear() {
    while (!empty())
        remove(first());
}

MoleculeCollection* MoleculeCollection::subCollection(size_t start, size_t end) const {
    MoleculeCollection* subCollection = new MoleculeCollection();
    tgtAssert(start <= end, "invalid indices");
    tgtAssert(start < moleculeHandles_.size(), "invalid start index");
    tgtAssert(end < moleculeHandles_.size(), "invalid end index");
    for (size_t index = start; index <= end; index++)
        subCollection->add(moleculeHandles_.at(index));
    return subCollection;
}

MoleculeCollection* MoleculeCollection::subCollection(const std::vector<size_t>& indices) const {
    MoleculeCollection* subCollection = new MoleculeCollection();
    for (size_t i=0; i<indices.size(); i++) {
        tgtAssert(indices.at(i) < moleculeHandles_.size(), "invalid index");
        subCollection->add(moleculeHandles_.at(indices.at(i)));
    }
    return subCollection;
}


size_t MoleculeCollection::size() const {
    return moleculeHandles_.size();
}

bool MoleculeCollection::empty() const {
    return (size() == 0);
}

void MoleculeCollection::notifyMoleculeAdded(const Molecule* handle) {
    const vector<MoleculeCollectionObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeAdded(this, handle);
}

void MoleculeCollection::notifyMoleculeRemoved(const Molecule* handle) {
    const vector<MoleculeCollectionObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeRemoved(this, handle);
}

void MoleculeCollection::notifyMoleculeChanged(const Molecule* handle) {
    const vector<MoleculeCollectionObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeChanged(this, handle);
}

// implementation of MoleculeObserver interface
void MoleculeCollection::moleculeChange(const Molecule* handle) {
    if (contains(handle))
        notifyMoleculeChanged(handle);

}

// implementation of MoleculeObserver interface
void MoleculeCollection::moleculeDelete(const Molecule* handle) {
    if (contains(handle))
        remove(handle);
}

} // namespace
