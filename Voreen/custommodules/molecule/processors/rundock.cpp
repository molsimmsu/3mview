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
    , checkButton_("checkButton", "check status")

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
    addProperty(checkButton_);
    
    selectTarget_.addOption("1", "first");
    selectTarget_.addOption("2", "second");
    runInterface_.addOption("local", "Local");
    dockType_.addOption("standart", "Standart");

    submitButton_.onChange(CallMemberAction<RunDock>(this, &RunDock::submit));
    readButton_.onChange(CallMemberAction<RunDock>(this, &RunDock::loadResult));
    checkButton_.onChange(CallMemberAction<RunDock>(this, &RunDock::checkStatus));
    readButton_.setVisible(false);
    checkButton_.setVisible(false);
   // checkButton_.setVisible(false);
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
        status_.set("select 2 molecules");
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
    molName1_ = collection->at(0)->getOrigin().getFilename();
    molName2_ = collection->at(1)->getOrigin().getFilename();
    
    
    std::string cmd("cp ../resource/scripts/runhexdock.sh ");
	cmd += workingDir_.get();
	cmd += "/. && cd ";
	cmd += workingDir_.get();
	cmd += "/ && ./runhexdock.sh ";
	
	if (selectTarget_.isSelected("1")==true)  
	    cmd +=  molName1_ + " " + molName2_ + " " ;
    else 
        cmd +=  molName2_ + " " + molName1_ + " " ;
    std::stringstream out;
    out << numMod_.get();
    cmd += out.str(); 
	
	FILE* pipe = popen(cmd.c_str(), "r");

	while (!feof(pipe))
	{
		char field[20]; float pident;
		fscanf(pipe, "%s", field);
		status_.set(std::string(field));
	}
	
	pclose(pipe);
    
    submitButton_.setVisible(false);
    readButton_.setVisible(false);
    checkButton_.setVisible(true);
    workingDir_.setVisible(false);
    moleculeURLlist_.setVisible(false);
    runInterface_.setVisible(false);
    numMod_.setVisible(false);
    selectTarget_.setVisible(false);
    dockType_.setVisible(false);
    
}

void RunDock::checkStatus(){
    bool complete = false;
    bool status = false;
    std::string cmd("cd ");
	cmd += workingDir_.get();
	cmd += "/ && tail -n 1 log ";
	
	FILE* pipe = popen(cmd.c_str(), "r");

	while (!feof(pipe))
	{
		char field[10]; float pident;
		fscanf(pipe, "%s", field);
		if(std::strcmp(field,"stopping:")==0) complete = true;
	}
	pclose(pipe);
	
	std::string cmd1("ps cax | grep 'hex6' > /dev/null; if [ $? -eq 0 ]; then echo yes; else  echo no; fi");
	
	FILE* pipe1 = popen(cmd1.c_str(), "r");

	while (!feof(pipe1))
	{
		char field[10]; float pident;
		fscanf(pipe, "%s", field);
		if(std::strcmp(field,"yes")==0) status = true;
	}
	pclose(pipe1);
	
	if(status){
	    status_.set(std::string("Hex is working"));
	}
	else {
	    if(complete){
	        status_.set("Task complete");
	        readButton_.setVisible(true);
	        checkButton_.setVisible(false);
	    }
	    else{
	    status_.set("Failure");
	    submitButton_.setVisible(true);
        readButton_.setVisible(false);
        workingDir_.setVisible(true);
        moleculeURLlist_.setVisible(true);
        runInterface_.setVisible(true);
        numMod_.setVisible(true);
        selectTarget_.setVisible(true);
        dockType_.setVisible(true);
	    checkButton_.setVisible(false);
	    }
	}
	
}


void RunDock::loadResult(){
    MoleculeCollectionSource* molCollection = getSourceProcessor();
    std::string loadName;
    if (molCollection == 0) return;
        if (selectTarget_.isSelected("1")==true)  
            loadName = molName2_;
        else 
            loadName = molName1_;
    loadName.erase(loadName.end()-4,loadName.end());
     
    std::string path(workingDir_.get() + "/");
    
    LINFO("Loading results:");
    for (size_t i = 1; i <= numMod_.get(); i++) {
        std::stringstream pdbPath;
        pdbPath << path << loadName << "_dock_" << i << "_.pdb" ;
        LINFO(pdbPath.str());
        molCollection->load(pdbPath.str(), true);
    }
    submitButton_.setVisible(true);
    readButton_.setVisible(false);
    checkButton_.setVisible(false);
    workingDir_.setVisible(true);
    moleculeURLlist_.setVisible(true);
    runInterface_.setVisible(true);
    numMod_.setVisible(true);
    selectTarget_.setVisible(true);
    dockType_.setVisible(true);
    status_.set("");
}
