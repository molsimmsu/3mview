#include "molmolalign.h"


const std::string MolMolAlign::loggerCat_("3MTK.MolMolAlign");


MolMolAlign :: MolMolAlign()
  : Processor()
  , tobealigned_("tobealigned", "Volume to reorientate", Processor::INVALID_PROGRAM)
  , align_("align", "Align", Processor::INVALID_PROGRAM)
  , moleculeURLlist_("moleculeURLlist_", "Molecule list", std::vector<std::string>())

{
    tobealigned_.addOption("Mol1ToMol2", "Molecule 1 to Molecule 2");
    tobealigned_.addOption("Mol2ToMol1", "Molecule 2 to Molecule 1");
    tobealigned_.addOption("Mol1ToOrigin", "Molecule 1 to Origin");
    tobealigned_.addOption("Mol2ToOrigin", "Molecule 2 to Origin");
    
    addProperty(moleculeURLlist_);
    addProperty(tobealigned_);
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
		const Molecule* firstMol;
		const Molecule* secondMol;
		
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

		const tgt::mat4  _temp = wrld2*invertednorm2*norm1*invertedwrld1;	
		outMol->transform(_temp);

        std::string url1 = firstMol->getOrigin().getURL();
        std::string url2 = secondMol->getOrigin().getFilename();
        std::string newOrigin = url1 + "_align_to_" + url2;
        outMol->setOrigin(VolumeURL(newOrigin));
        
        std::stringstream info;
        info << "Output origin: " << newOrigin;
        LINFO(info);
        
		getSourceProcessor()->addMolecule(outMol, true, true);
	}

	if (tobealigned_.isSelected("Mol1ToOrigin") || tobealigned_.isSelected("Mol2ToOrigin"))
	{
		const Molecule* molecula;
		if (tobealigned_.isSelected("Mol1ToOrigin")) 
		    molecula = molecules->at(0);
		if (tobealigned_.isSelected("Mol2ToOrigin")) 
		    molecula = molecules->at(1);


	    Molecule* outMol = molecula->clone(); 

 		tgt::mat4 wrld = outMol->getTransformationMatrix();
 		tgt::mat4 inv;
		wrld.invert(inv);

          LINFO("Calculating transformation matrix for object...");
		tgt::mat4 fit  = GetAlignment(outMol);

		const tgt::mat4  _temp = fit*inv;		
		outMol->transform(_temp);
		std :: cout << "Done. \n";
		
        std::string url1 = molecula->getOrigin().getURL();
        std::string newOrigin = url1 + "_align_to_origin";
        outMol->setOrigin(VolumeURL(newOrigin));
        
        std::stringstream info;
        info << "Output origin: " << newOrigin;
        LINFO(info);
        
		getSourceProcessor()->addMolecule(outMol, true, true);
	}

}

tgt::mat4 MolMolAlign :: GetAlignment(const Molecule* mol)
{
	PointCloud cloud;
	cloud.MoleculeFill(mol);

	cloud.scale    = 32;
	cloud.max_size = 1.5;

	tgt::Matrix4d result = cloud.GetShift();
	result = cloud.GetAxes()*result;
	return result;
}



