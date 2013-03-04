#include "moleculeselector.h"

#include "../datastructures/molecule.h"
#include "../datastructures/moleculecollection.h"

namespace voreen {

const std::string MoleculeSelector::loggerCat_("voreen.core.MoleculeSelector");

MoleculeSelector::MoleculeSelector()
    : Processor(),
      moleculeID_("moleculeID", "Selected molecule", 0, 0, 100),
      inport_(Port::INPORT, "moleculecollection", "MoleculeCollection Input", false),
      outport_(Port::OUTPORT, "moleculehandle.moleculehandle", "Molecule Output", false)
{
    addPort(inport_);
    addPort(outport_);

    addProperty(moleculeID_);
}

Processor* MoleculeSelector::create() const {
    return new MoleculeSelector();
}

void MoleculeSelector::process() {
    // nothing
}

void MoleculeSelector::initialize() throw (tgt::Exception) {
    Processor::initialize();

    adjustToMoleculeCollection();
}

void MoleculeSelector::invalidate(int /*inv = INVALID_RESULT*/) {
    adjustToMoleculeCollection();
}

void MoleculeSelector::adjustToMoleculeCollection() {
    if (!outport_.isInitialized())
        return;

    const MoleculeCollection* collection = inport_.getData();
    int max = ((collection != 0) ? static_cast<int>(collection->size()) : 0);

    if (collection && !collection->empty() && (moleculeID_.get() < max)) {
        // adjust max id to size of collection
        if (moleculeID_.getMaxValue() != max - 1) {
            moleculeID_.setMaxValue(max - 1);
            if (moleculeID_.get() > moleculeID_.getMaxValue())
                moleculeID_.set(moleculeID_.getMaxValue());
            moleculeID_.updateWidgets();
        }

        tgtAssert((moleculeID_.get() >= 0) && (moleculeID_.get() < max), "Invalid molecule index");

        // update output handle
        //if (collection->at(moleculeID_.get()) != outport_.getData())
            outport_.setData(collection->at(moleculeID_.get()), false);
    }
    else {
        if (max > 0)
            max--;

        // If the collection is smaller than the previous one, the maximum value
        // must be adjusted and the new value should be set.
        // The collection is 0 when deserializing the workspace, so that we must
        // not set value in that case, because it is the just deserialized one!
        moleculeID_.setMaxValue(max);
        if (collection != 0 && !collection->empty()) {
            moleculeID_.set(max);
            if (static_cast<int>(collection->size()) > moleculeID_.get()
                //&& collection->at(moleculeID_.get()) != outport_.getData()
                )
            {
                outport_.setData(collection->at(moleculeID_.get()), false);
            }
        } else {
            outport_.setData(0);
        }

        moleculeID_.updateWidgets();
    }
}

} // namespace
