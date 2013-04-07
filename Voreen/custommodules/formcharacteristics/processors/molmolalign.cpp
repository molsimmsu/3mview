#include "molmolalign.h"


const std::string MolMolAlign::loggerCat_("3MTK.MolMolAlign");


MolMolAlign :: MolMolAlign()
  : Processor()
  , tobealigned_("tobealigned", "Volume to reorientate", Processor::INVALID_PROGRAM)
  , align_("align", "Align", Processor::INVALID_PROGRAM)
  , createNew_("createNew_", "Create new molecule", false)
  , moleculeURLlist_("moleculeURLlist_", "Molecule list", std::vector<std::string>())

{
    tobealigned_.addOption("Mol1ToMol2", "Molecule 1 to Molecule 2");
    tobealigned_.addOption("Mol2ToMol1", "Molecule 2 to Molecule 1");
    tobealigned_.addOption("SelectedToOrigin", "Selected to Origin");
    
    addProperty(moleculeURLlist_);
    addProperty(tobealigned_);
    addProperty(createNew_);
    addProperty(align_);
    
    align_.onClick(CallMemberAction<MolMolAlign>(this, &MolMolAlign::align));
}

void MolMolAlign::updateSelection() {
    //LWARNING("MoleculeManipulation::updateSelection()");
    MoleculeCoProcessor::updateSelection();
    const MoleculeCollection* collection = getInputMoleculeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    moleculeURLlist_.clear();
    for (size_t i = 0; i < collection->size(); i++) {
        moleculeURLlist_.addMolecule(collection->at(i));
        //LWARNING("MoleculeManipulation::updateSelection() added Molecule");
    }
}

void MolMolAlign :: align()
{
    MoleculeCollection* molecules = moleculeURLlist_.getMolecules(true);
    
	if (tobealigned_.isSelected("Mol1ToMol2") || tobealigned_.isSelected("Mol2ToMol1"))
	{
		Molecule* firstMol;
		Molecule* secondMol;
		
		if (tobealigned_.isSelected("Mol1ToMol2")) {
		    firstMol  = molecules->at(0);
		    secondMol = molecules->at(1);
		}
		if (tobealigned_.isSelected("Mol2ToMol1")) {
		    firstMol  = molecules->at(1);
		    secondMol = molecules->at(0);
		}

		tgt::mat4 norm1;
		tgt::mat4 norm2;
		tgt::mat4 invertednorm2;
		tgt::mat4 wrld1;
		tgt::mat4 wrld2;
		tgt::mat4 invertedwrld1;


 	    Molecule* outMol = firstMol->clone();
	    LINFO("Getting transformation matrix for molecule #1..");
		wrld1 = outMol->getTransformationMatrix();
		norm1 = GetAlignment(outMol);
		wrld1.invert(invertedwrld1);

        LINFO("Getting transformation matrix for molecole #2..");

		wrld2 = secondMol->getTransformationMatrix();
		norm2 = GetAlignment(secondMol);
		norm2.invert(invertednorm2);

		const tgt::mat4 _temp = wrld2*invertednorm2*norm1*invertedwrld1;
		
		if (createNew_.get() == true) {
		    outMol->transform(_temp);

            std::string url1 = firstMol->getOrigin().getURL();
            std::string url2 = secondMol->getOrigin().getFilename();
            size_t dotPos = url1.find_last_of('.');
            url1 = url1.substr(0, dotPos);
            std::string newOrigin = url1 + "_align_to_" + url2;
            outMol->setOrigin(VolumeURL(newOrigin));
            
		    getSourceProcessor()->addMolecule(outMol, true, true);
		}
		else {
		    firstMol->setTransformationMatrix(tgt::mat4::createIdentity());
		    firstMol->transform(_temp);
		    delete outMol;
		}
	}

	if (tobealigned_.isSelected("SelectedToOrigin"))
	for (size_t i = 0; i < molecules->size(); i++)
	{
		Molecule* molecula = molecules->at(i);

	    Molecule* outMol = molecula->clone(); 

 		tgt::mat4 wrld = outMol->getTransformationMatrix();
 		tgt::mat4 inv;
		wrld.invert(inv);

        LINFO("Calculating transformation matrix for object...");
		tgt::mat4 fit  = GetAlignment(outMol);

		const tgt::mat4  _temp = fit*inv;
		
		if (createNew_.get() == true) {
		    outMol->transform(_temp);
		    std :: cout << "Done. \n";
		
            std::string url1 = molecula->getOrigin().getURL();
            size_t dotPos = url1.find_last_of('.');
            url1 = url1.substr(0, dotPos);
            std::string newOrigin = url1 + "_align_to_origin.pdb";
            outMol->setOrigin(VolumeURL(newOrigin));
            
            std::stringstream info;
            info << "Output origin: " << newOrigin;
            LINFO(info);
            
		    getSourceProcessor()->addMolecule(outMol, true, true);
		}
		else {
		    molecula->setTransformationMatrix(tgt::mat4::createIdentity());
		    molecula->transform(_temp);
		    delete outMol;
		}
	}

}

tgt::mat4 MolMolAlign :: GetAlignment(const Molecule* mol)
{
	PointCloud cloud;
	cloud.MoleculeFill(mol);
	tgt::Matrix4d result = cloud.GetShift();
	result = cloud.GetAxes()*result;
	return result;
}



