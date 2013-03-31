#include "moleculeselectornores.h"

#include "../datastructures/molecule.h"
#include "../datastructures/moleculecollection.h"

namespace voreen {

const std::string MoleculeSelectornores::loggerCat_("voreen.core.MoleculeSelectornores");

MoleculeSelectornores::MoleculeSelectornores()
    : Processor(),
      moleculeID_("moleculeID", "Selected molecule", 0, 0, 100),
      inport_(Port::INPORT, "moleculecollection", "MoleculeCollection Input", false),
      outport_(Port::OUTPORT, "moleculehandle.moleculehandle", "Molecule Output", false),
      resType_("restype", "select residues"),
      invertSelection_("invertSelection", "Invert Selection", false)

{
    resType_.addOption("water", "Water");
    resType_.addOption("protein", "Protein");
    resType_.addOption("nucleo", "Nucleo");
    resType_.addOption("ion", "Ion");
    resType_.addOption("solvent", "Solvent");
    
    addProperty(resType_);
    addProperty(invertSelection_);
    addPort(inport_);
    addPort(outport_);

    addProperty(moleculeID_);
    resType_.onChange(CallMemberAction<MoleculeSelectornores>(this, &MoleculeSelectornores::UpdateResSelection));
    invertSelection_.onChange(CallMemberAction<MoleculeSelectornores>(this, &MoleculeSelectornores::UpdateResSelection));
}

Processor* MoleculeSelectornores::create() const {
    return new MoleculeSelectornores();
}

void MoleculeSelectornores::process() {
    // nothing
}

void MoleculeSelectornores::initialize() throw (tgt::Exception) {
    Processor::initialize();

    adjustToMoleculeCollection();
}

void MoleculeSelectornores::invalidate(int /*inv = INVALID_RESULT*/) {
    adjustToMoleculeCollection();
}

void MoleculeSelectornores::UpdateResSelection() {
int resid=9;
    if(resType_.isSelected("water")) resid=9;
    else if(resType_.isSelected("protein")) resid=5;
    else if(resType_.isSelected("nucleo")) resid=4;
    else if(resType_.isSelected("ion")) resid=3;
    else if(resType_.isSelected("solvent")) resid=8;
    const MoleculeCollection* collection = inport_.getData();
    Molecule *mol = collection->at(moleculeID_.get())->clone();
    mol->clearResidues(resid, invertSelection_.get());
    outport_.setData(mol, false);
}

void MoleculeSelectornores::adjustToMoleculeCollection() {
    if (!outport_.isInitialized())
        return;
    int resid=9;
    if(resType_.isSelected("water")) resid=9;
    else if(resType_.isSelected("protein")) resid=5;
    else if(resType_.isSelected("nucleo")) resid=4;
    else if(resType_.isSelected("ion")) resid=3;
    else if(resType_.isSelected("solvent")) resid=8;
    
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
            Molecule *mol = collection->at(moleculeID_.get())->clone();
                mol->clearResidues(resid, invertSelection_.get());
                outport_.setData(mol, false);
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
                Molecule *mol = collection->at(moleculeID_.get())->clone();
                mol->clearResidues(resid, invertSelection_.get());
                outport_.setData(mol, false);
            }
        } else {
            outport_.setData(0);
        }

        moleculeID_.updateWidgets();
    }
}

} // namespace
