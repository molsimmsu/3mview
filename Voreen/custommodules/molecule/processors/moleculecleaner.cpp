#include "moleculecleaner.h"

#include "../datastructures/molecule.h"
#include "../datastructures/moleculecollection.h"

const std::string MoleculeCleaner::loggerCat_("3MTK.MoleculeCleaner");

MoleculeCleaner::MoleculeCleaner()
    : resType_("restype", "select residues")
    , invertSelection_("invertSelection", "Invert Selection", false)
    , removeHydrogens_("removeHydrogens", "Remove Hydrogens", true)
    , createNew_("createNew", "Create new molecule", true)
    , clearButton_("clearButton", "Clear selected")
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())
{
    resType_.addOption("water", "Water");
    resType_.addOption("protein", "Protein");
    resType_.addOption("nucleo", "Nucleo");
    resType_.addOption("ion", "Ion");
    resType_.addOption("solvent", "Solvent");
    
    addProperty(moleculeURLlist_);
    addProperty(resType_);
    addProperty(invertSelection_);
    addProperty(removeHydrogens_);
    addProperty(createNew_);
    addProperty(clearButton_);
    
    clearButton_.onChange(CallMemberAction<MoleculeCleaner>(this, &MoleculeCleaner::UpdateResSelection));
}

Processor* MoleculeCleaner::create() const {
    return new MoleculeCleaner();
}

void MoleculeCleaner::updateSelection() {
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

void MoleculeCleaner::UpdateResSelection() {
    int resid=9;
    if(resType_.isSelected("water")) resid=9;
    else if(resType_.isSelected("protein")) resid=5;
    else if(resType_.isSelected("nucleo")) resid=4;
    else if(resType_.isSelected("ion")) resid=3;
    else if(resType_.isSelected("solvent")) resid=8;
    
    MoleculeCollection* collection = moleculeURLlist_.getMolecules(true);
    
    for (size_t i = 0; i < collection->size(); i++) {
        Molecule *mol = collection->at(i);
        if (createNew_.get() == true) {
            Molecule* newMol = mol->clone();
            newMol->clearResidues(resid, invertSelection_.get());
            
            std::string url = mol->getOrigin().getURL();
            newMol->setOrigin(VolumeURL(url + "_no_" + resType_.get()));
            
            getSourceProcessor()->addMolecule(newMol, true, true);
        }
        else
            mol->clearResidues(resid, invertSelection_.get());
    }

}

