#include "moleculesaver.h"

#include "../datastructures/molecule.h"
#include "../datastructures/moleculecollection.h"
#include "../utils/moleculeio.h"


#include <fstream>
#include <string>

const std::string MoleculeSaver::loggerCat_("3MTK.MoleculeSaver");

MoleculeSaver::MoleculeSaver()
    : mergeMolecules_("mergeMolecules", "Merge molecules ", false)
    , folder_("saveToFolder", "Folder", "Selectfolder", VoreenApplication::app()->getUserDataPath(), "PDB (*.pdb)", FileDialogProperty::DIRECTORY)
    , saveButton_("saveButton", "Save")
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())

{
    addProperty(folder_);
    addProperty(moleculeURLlist_);
    addProperty(mergeMolecules_);
    addProperty(saveButton_);

    saveButton_.onChange(CallMemberAction<MoleculeSaver>(this, &MoleculeSaver::SaveMolecules));
}

Processor* MoleculeSaver::create() const {
    return new MoleculeSaver();
}

void MoleculeSaver::updateSelection() {
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

void MoleculeSaver::SaveMolecules() {
    
    MoleculeCollection* collection = moleculeURLlist_.getMolecules(true);
    
    for (size_t i = 0; i < collection->size(); i++) {
        Molecule *mol = collection->at(i);
        if (mergeMolecules_.get() == false) {
            std::string url = mol->getOrigin().getFilename();
            std::cout << folder_.get() << std::endl;
            MoleculeIO::write(mol, folder_.get() + "/" + url);        
        }
        //else
          //  ;
    } 

}

