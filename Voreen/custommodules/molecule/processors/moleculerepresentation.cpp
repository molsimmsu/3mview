#include "moleculerepresentation.h"

#include "../datastructures/molecule.h"
#include "../datastructures/moleculecollection.h"

const std::string MoleculeRepresentation::loggerCat_("3MTK.MoleculeRepresentation");

MoleculeRepresentation::MoleculeRepresentation()
    : repType_("repType", "Representation")
    , selectButton_("selectButton", "Set representation")
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())
{
    repType_.addOption("BallsAndSticks", "Balls and sticks");
    repType_.addOption("Ribbons", "Ribbons");
    repType_.addOption("Surface", "Surface");
    
    addProperty(moleculeURLlist_);
    addProperty(repType_);
    addProperty(selectButton_);
    
    selectButton_.onChange(CallMemberAction<MoleculeRepresentation>(this, &MoleculeRepresentation::updateRepresentation));
}

Processor* MoleculeRepresentation::create() const {
    return new MoleculeRepresentation();
}

void MoleculeRepresentation::updateSelection() {
    MoleculeCoProcessor::updateSelection();
    const MoleculeCollection* collection = getInputMoleculeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    moleculeURLlist_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        moleculeURLlist_.addMolecule(collection->at(i));
}

void MoleculeRepresentation::updateRepresentation() {
    MoleculeCollection* collection = moleculeURLlist_.getMolecules(true);
    
    for (size_t i = 0; i < collection->size(); i++) {
        Molecule *mol = collection->at(i);

        if (repType_.get() == "BallsAndSticks") mol->setRepresentation(new BallsAndSticksRep());
        else if (repType_.get() == "Ribbons") mol->setRepresentation(new RibbonsRep());
        else if (repType_.get() == "Surface") mol->setRepresentation(new SurfaceRep());
    }

}

