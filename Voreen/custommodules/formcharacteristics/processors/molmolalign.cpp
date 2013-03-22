#include "molmolalign.h"


const std::string MolMolAlign::loggerCat_("3mview.MolMolAlign");


MolMolAlign :: MolMolAlign()
  : Processor(),
    tobealigned_("tobealigned", "Volume to reorientate", Processor::INVALID_PROGRAM),
    align_("align", "Align", Processor::INVALID_PROGRAM),
    molinport1_(Port::INPORT,   "molecule1", "Molecule 1"),
    molinport2_(Port::INPORT,   "molecule2", "Molecule 2"),
    outport_(Port::OUTPORT,   "molecule3", "Molecule")
{
    tobealigned_.addOption("Mol1ToMol2", "Molecule 1 to Molecule 2");
    tobealigned_.addOption("Mol2ToMol1", "Molecule 2 to Molecule 1");
    tobealigned_.addOption("Mol1ToOrigin", "Molecule 1 to Origin");
    tobealigned_.addOption("Mol2ToOrigin", "Molecule 2 to Origin");
    addProperty(tobealigned_);
    addProperty(align_);

    addPort(molinport1_);
    addPort(molinport2_);
    addPort(outport_);
    
    align_.onClick(CallMemberAction<MolMolAlign>(this, &MolMolAlign::align));
}

void MolMolAlign :: align()
{
	if (tobealigned_.isSelected("Mol1ToMol2") || tobealigned_.isSelected("Mol2ToMol1"))
	{
		const Molecule* firstMol;
		const Molecule* secondMol;
		
		if (tobealigned_.isSelected("Mol1ToMol2")) {
		    firstMol  = molinport1_.getData();
		    secondMol = molinport2_.getData();
		}
		if (tobealigned_.isSelected("Mol2ToMol1")) {
		    firstMol  = molinport2_.getData();
		    secondMol = molinport1_.getData();
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
		outport_.setData(outMol);
	}

	if (tobealigned_.isSelected("Mol1ToOrigin") || tobealigned_.isSelected("Mol2ToOrigin"))
	{
		const Molecule* molecula;
		if (tobealigned_.isSelected("Mol1ToOrigin")) 
		    molecula = molinport1_.getData();
		if (tobealigned_.isSelected("Mol2ToOrigin")) 
		    molecula = molinport2_.getData();


	     Molecule* outMol = molecula->clone(); 

 		tgt::mat4 wrld = outMol->getTransformationMatrix();
 		tgt::mat4 inv;
		wrld.invert(inv);

          LINFO("Calculating transformation matrix for object...");
		tgt::mat4 fit  = GetAlignment(outMol);

		const tgt::mat4  _temp = fit*inv;		
		outMol->transform(_temp);
		std :: cout << "Done. \n";
		outport_.setData(outMol);
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



