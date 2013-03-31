#include "volvolalign.h"
#include "../ext/tgt/matrix.h"
#include <sstream>

const std::string VolVolAlign::loggerCat_("3mview.VolVolAlign");


VolVolAlign :: VolVolAlign()
  : Processor()
  , tobealigned_("tobealigned", "Volume to reorientate", Processor::INVALID_PROGRAM)
  , align_("align", "Align", Processor::INVALID_PROGRAM)
  , volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
{
    tobealigned_.addOption("Vol1ToVol2", "Volume 1 to Volume 2");
    tobealigned_.addOption("Vol2ToVol1", "Volume 2 to Volume 1");
    tobealigned_.addOption("Vol1ToOrigin", "Volume 1 to Origin");
    tobealigned_.addOption("Vol2ToOrigin", "Volume 2 to Origin");
    
    addProperty(volumeURLList_);
    addProperty(tobealigned_);
    addProperty(align_);
    
    align_.onClick(CallMemberAction<VolVolAlign>(this, &VolVolAlign::align));
}

void VolVolAlign :: updateSelection() {
    DensityMapCoProcessor::updateSelection();
    const VolumeCollection* collection = getInputVolumeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    volumeURLList_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        volumeURLList_.addVolume(collection->at(i));
}

void VolVolAlign :: align()
{
    VolumeCollection* volumes = volumeURLList_.getVolumes(true);
    
	if (tobealigned_.isSelected("Vol1ToVol2") || tobealigned_.isSelected("Vol2ToVol1"))
	{
		const VolumeBase* firstVolume;
		const VolumeBase* secondVolume;
		
		if (tobealigned_.isSelected("Vol1ToVol2")) {
		    firstVolume  = volumes->at(0);
		    secondVolume = volumes->at(1);
		}
		
		if (tobealigned_.isSelected("Vol2ToVol1")) {
		    firstVolume  = volumes->at(1);
		    secondVolume = volumes->at(0);
		}
		
		if (firstVolume == 0 || secondVolume == 0) {
		    LWARNING("Some of the input volumes is 0");
		    return;
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
        
        std::string url1 = firstVolume->getOrigin().getURL();
        std::string url2 = secondVolume->getOrigin().getFilename();
        std::string newOrigin = url1 + "_align_to_" + url2;
        combinedVolume->setOrigin(VolumeURL(newOrigin));
        
        std::stringstream info;
        info << "Output origin: " << newOrigin;
        LINFO(info);
        
		getSourceProcessor()->addVolume(combinedVolume, true, true);
	}

	if (tobealigned_.isSelected("Vol1ToOrigin") || tobealigned_.isSelected("Vol2ToOrigin"))
	{
		const VolumeBase* volume;
		if (tobealigned_.isSelected("Vol1ToOrigin")) 
		    volume = volumes->at(0);
		if (tobealigned_.isSelected("Vol2ToOrigin")) 
		    volume = volumes->at(1);
		    
		if (volume == 0) {
		    LWARNING("Input volume is 0");
		    return;
		}

 	    Volume* combinedVolume = volume->clone();	    
		tgt::Matrix4d wrld = combinedVolume->getVoxelToWorldMatrix();
  	    LINFO("Getting transformation matrix for object..");

		tgt::Matrix4d norm = GetAlignment(combinedVolume);
		combinedVolume->setPhysicalToWorldMatrix(norm*wrld);

		combinedVolume->setOffset (tgt::vec3(0, 0, 0));
		combinedVolume->setSpacing(tgt::vec3(1, 1, 1));

        std::string url = volume->getOrigin().getURL();
        combinedVolume->setOrigin(VolumeURL(url + "_align_to_origin"));
		getSourceProcessor()->addVolume(combinedVolume, true, true);
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


