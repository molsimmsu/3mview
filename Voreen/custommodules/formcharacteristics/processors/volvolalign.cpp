#include "volvolalign.h"
#include "../ext/tgt/matrix.h"


const std::string VolVolAlign::loggerCat_("3mview.VolVolAlign");


VolVolAlign :: VolVolAlign()
  : Processor(),
    tobealigned_("tobealigned", "Volume to reorientate", Processor::INVALID_PROGRAM),
    align_("align", "Align", Processor::INVALID_PROGRAM),
    volinport1_(Port::INPORT,   "volume1", "Electon density map 1"),
    volinport2_(Port::INPORT,   "volume2", "Electon density map 2"),
    outport_(Port::OUTPORT,   "volume3", "Electon density map")
{
    tobealigned_.addOption("Vol1ToVol2", "Volume 1 to Volume 2");
    tobealigned_.addOption("Vol2ToVol1", "Volume 2 to Volume 1");
    tobealigned_.addOption("Vol1ToOrigin", "Volume 1 to Origin");
    tobealigned_.addOption("Vol2ToOrigin", "Volume 2 to Origin");
    addProperty(tobealigned_);
    addProperty(align_);

    addPort(volinport1_);
    addPort(volinport2_);
    addPort(outport_);
    
    align_.onClick(CallMemberAction<VolVolAlign>(this, &VolVolAlign::align));
}

void VolVolAlign :: align()
{
	if (tobealigned_.isSelected("Vol1ToVol2") || tobealigned_.isSelected("Vol2ToVol1"))
	{
		const VolumeBase* firstVolume;
		const VolumeBase* secondVolume;
		
		if (tobealigned_.isSelected("Vol1ToVol2")) {
		    firstVolume  = volinport1_.getData();
		    secondVolume = volinport2_.getData();
		}
		if (tobealigned_.isSelected("Vol2ToVol1")) {
		    firstVolume  = volinport2_.getData();
		    secondVolume = volinport1_.getData();
		}

 	     Volume* combinedVolume = firstVolume->clone();	    
		tgt::Matrix4d wrld1 = combinedVolume->getVoxelToWorldMatrix();
  	     LINFO("Getting transformation matrix for object..");
 	     
		tgt::Matrix4d norm1 = GetAlignment(combinedVolume);
		combinedVolume->setPhysicalToWorldMatrix(norm1*wrld1);

		combinedVolume->setOffset (tgt::vec3(0, 0, 0));
		combinedVolume->setSpacing(tgt::vec3(1, 1, 1));

 	     Volume* temp = secondVolume->clone();	    
		tgt::Matrix4d wrld2 = temp->getVoxelToWorldMatrix();	

		tgt::Matrix4d inv2;
		tgt::Matrix4d norm2 = GetAlignment(temp);
		temp->setPhysicalToWorldMatrix(norm2*wrld2);

		norm2.invert(inv2);
 
		combinedVolume->setPhysicalToWorldMatrix(inv2*norm1*wrld1);
		outport_.setData(combinedVolume);
	}

	if (tobealigned_.isSelected("Vol1ToOrigin") || tobealigned_.isSelected("Vol2ToOrigin"))
	{
		const VolumeBase* volume;
		if (tobealigned_.isSelected("Vol1ToOrigin")) 
		    volume = volinport1_.getData();
		if (tobealigned_.isSelected("Vol2ToOrigin")) 
		    volume = volinport2_.getData();


 	     Volume* combinedVolume = volume->clone();	    
		tgt::Matrix4d wrld = combinedVolume->getVoxelToWorldMatrix();
  	     LINFO("Getting transformation matrix for object..");

		tgt::Matrix4d norm = GetAlignment(combinedVolume);
		combinedVolume->setPhysicalToWorldMatrix(norm*wrld);

		combinedVolume->setOffset (tgt::vec3(0, 0, 0));
		combinedVolume->setSpacing(tgt::vec3(1, 1, 1));

		outport_.setData(combinedVolume);
	}
}

tgt::Matrix4d VolVolAlign :: GetAlignment(const Volume* vol)
{
	PointCloud cloud;

	cloud.VolumeFill(vol);
	cloud.scale    = 32;
	cloud.max_size = 1.5;

	tgt::Matrix4d result = cloud.GetShift();
	result = cloud.GetAxes()*result;
	return result;
}


