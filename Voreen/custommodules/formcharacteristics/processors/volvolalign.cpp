#include "volvolalign_.h"
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
 	     
		tgt::Matrix4d shift1 = GetShift(combinedVolume);
		combinedVolume->setPhysicalToWorldMatrix(shift1*wrld1);
		tgt::Matrix4d rotate1= GetAxes();
		combinedVolume->setPhysicalToWorldMatrix(rotate1*shift1*wrld1);

		combinedVolume->setOffset (tgt::vec3(0, 0, 0));
		combinedVolume->setSpacing(tgt::vec3(1, 1, 1));



 	     Volume* temp = secondVolume->clone();	    
		tgt::Matrix4d wrld2 = temp->getVoxelToWorldMatrix();	

		tgt::Matrix4d invrot, invshift;
		tgt::Matrix4d shift2 = GetShift(temp);
		temp->setPhysicalToWorldMatrix(shift2*wrld2);
		tgt::Matrix4d rotate2= GetAxes();

		shift2.invert(invshift);
		rotate2.invert(invrot);
 
		combinedVolume->setPhysicalToWorldMatrix(invshift*invrot*rotate1*shift1*wrld1);
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
 	     
		tgt::Matrix4d shift = GetShift(combinedVolume);
		combinedVolume->setPhysicalToWorldMatrix(shift*wrld);
		tgt::Matrix4d rotate= GetAxes();
		combinedVolume->setPhysicalToWorldMatrix(rotate*shift*wrld);

		combinedVolume->setOffset (tgt::vec3(0, 0, 0));
		combinedVolume->setSpacing(tgt::vec3(1, 1, 1));

		outport_.setData(combinedVolume);
	}
}

tgt::Matrix4d VolVolAlign :: GetShift(const Volume* vol)
{
	cloud.VolumeFill(vol);
	cloud.scale    = 32;
	cloud.max_size = 1.5;

	tgt::Matrix4d out_data(cloud.GetShift());
	return out_data;
}

tgt::Matrix4d VolVolAlign :: GetAxes()
{
	
	tgt::Matrix4d out_data(cloud.GetAxes());
	return out_data;
}
