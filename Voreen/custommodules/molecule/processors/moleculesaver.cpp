#include "moleculesaver.h"

#include "../datastructures/molecule.h"
#include "../datastructures/moleculecollection.h"
#include "../utils/moleculeio.h"


#include <fstream>
#include <string>

const std::string MoleculeSaver::loggerCat_("3MTK.MoleculeSaver");

MoleculeSaver::MoleculeSaver()
    : mergeMolecules_("mergeMolecules", "Merge molecules ", false)
    , removeHeader_("removeHeader", "Remove PDB Header", true)
    , folder_("saveToFolder", "Folder", "Selectfolder", VoreenApplication::app()->getUserDataPath(), "PDB (*.pdb)", FileDialogProperty::DIRECTORY)
    , saveButton_("saveButton", "Save")
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())
    , mergedName_("mergedName","Merged molecule name","merged.pdb")
    

{
    addProperty(folder_);
    addProperty(moleculeURLlist_);
    addProperty(mergeMolecules_);
    addProperty(mergedName_);
    addProperty(removeHeader_);
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
    if(collection->size()>0){
        if (mergeMolecules_.get() == false) {
            for (size_t i = 0; i < collection->size(); i++) {
                Molecule *mol = collection->at(i)->clone();
                tgt::mat4 matrix = collection->at(i)->getTransformationMatrix();
                mol->setTransformationMatrix(matrix);
                std::cout << matrix << std::endl;
                
                mol->updateCoordinates();
                std::cout << mol->getTransformationMatrix() << std::endl;
                std::string url = collection->at(i)->getOrigin().getFilename();
                if(removeHeader_.get()==true) MoleculeIO::write(mol, folder_.get() + "/" + url, true);
                else MoleculeIO::write(mol, folder_.get() + "/" + url);
                delete mol;
            }
        }
       else{
            Molecule * merged = new Molecule;
            for (size_t i = 0; i < collection->size(); i++) {
                Molecule *mol = collection->at(i)->clone();
                mol->setTransformationMatrix(collection->at(i)->getTransformationMatrix());
                mol->updateCoordinates();
                merged->addMolecule(mol);
                delete mol;
            }
            std::string url = mergedName_.get();
            
            MoleculeIO::write(merged, folder_.get() + "/" + url);
        } 
    }
    else LINFO("Select molecules for saving");
}

