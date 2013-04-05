#include "rundock.h"

#include "../datastructures/molecule.h"
#include "../datastructures/moleculecollection.h"
#include "../utils/moleculeio.h"


#include <fstream>
#include <string>

const std::string RunDock::loggerCat_("3MTK.RunDock");

RunDock::RunDock()
    : workingDir_("saveToFolder", "Folder", "Selectfolder", VoreenApplication::app()->getUserDataPath(),
            "PDB (*.pdb)", FileDialogProperty::DIRECTORY)
    , moleculeURLlist_("moleculeURLlist_", "Molecule URL List", std::vector<std::string>())
    , runInterface_("runInterface", "Submit to", Processor::INVALID_PROGRAM)
    , numMod_("numMod", "Amount of models", 3, 1, 25)
    , selectTarget_("selectTarget", "Select receptor", Processor::INVALID_PROGRAM)
    , dockType_("dockType", "Docking type", Processor::INVALID_PROGRAM)
    , status_("status","Status","")
    , submitButton_("submitButton", "Submit")
    , readButton_("readButton", "Load results")

{
    addProperty(workingDir_);
    addProperty(moleculeURLlist_);
    addProperty(runInterface_);
    addProperty(numMod_);
    addProperty(selectTarget_);
    addProperty(dockType_);
    addProperty(status_);
    addProperty(submitButton_);
    addProperty(readButton_);
    
    selectTarget_.addOption("1", "first");
    selectTarget_.addOption("2", "second");
    runInterface_.addOption("local", "Local");
    dockType_.addOption("standart", "Standart");

    submitButton_.onChange(CallMemberAction<RunDock>(this, &RunDock::submit));
    readButton_.onChange(CallMemberAction<RunDock>(this, &RunDock::loadResult));
}

Processor* RunDock::create() const {
    return new RunDock();
}

void RunDock::updateSelection() {
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

void RunDock::submit() {
    
    MoleculeCollection* collection = moleculeURLlist_.getMolecules(true);
    if (collection == 0 || collection->size() == 0) {
            //LINFO("Exit DotLauncher::savePDBtoWorkDir() at return");
            return;
    }
        
    if (collection->size() != 2) {
        LINFO("You should select 2 molecules");
        return;
    }
    
    for (size_t i = 0; i < collection->size(); i++) {
        Molecule *mol = collection->at(i)->clone();
        tgt::mat4 matrix = collection->at(i)->getTransformationMatrix();
        mol->setTransformationMatrix(matrix);
        std::cout << matrix << std::endl;
 
        mol->updateCoordinates();
        //std::cout << mol->getTransformationMatrix() << std::endl;
        std::string url = workingDir_.get() + "/" + collection->at(i)->getOrigin().getFilename();
        MoleculeIO::write(mol,url, true);
        delete mol;
    }
    std::string molName1 = collection->at(0)->getOrigin().getFilename();
    std::string molName2 = collection->at(1)->getOrigin().getFilename();
    
    
    std::string cmd("cp ../resource/scripts/runhexdock.sh ");
	cmd += workingDir_.get();
	cmd += "/. && cd ";
	cmd += workingDir_.get();
	cmd += "/ && ./runhexdock.sh ";
	
	if (selectTarget_.isSelected("1")==true)  
	    cmd +=  molName1 + " " + molName2 + " " ;//+ //std::string(numMod_.get());
    else 
        cmd +=  molName2 + " " + molName1 + " " ;
    std::stringstream out;
    out << numMod_.get();
    cmd += out.str();  //+ std::string(numMod_.get());
	
	FILE* pipe = popen(cmd.c_str(), "r");

	while (!feof(pipe))
	{
		char field[20]; float pident;
		fscanf(pipe, "%s", field);
		status_.set(std::string(field));
	}
	
	pclose(pipe);
    
    
}


void RunDock::loadResult(){

}
