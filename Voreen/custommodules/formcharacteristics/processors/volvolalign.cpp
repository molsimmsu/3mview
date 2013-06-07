#include "volvolalign.h"
#include "../ext/tgt/matrix.h"
#include <sstream>

const std::string VolVolAlign::loggerCat_("3mview.VolVolAlign");


VolVolAlign :: VolVolAlign()
  : Processor()
  , tobealigned_("tobealigned", "Volume to reorientate", Processor::INVALID_PROGRAM)
  , align_("align", "Align", Processor::INVALID_PROGRAM)
  , createNew_("createNew", "Create new volume")
  , volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
{
    tobealigned_.addOption("Vol1ToVol2", "Volume 1 to Volume 2");
    tobealigned_.addOption("Vol2ToVol1", "Volume 2 to Volume 1");
    tobealigned_.addOption("SelectedToOrigin", "Selected to Origin");
    
    addProperty(volumeURLList_);
    addProperty(tobealigned_);
    addProperty(createNew_);
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
    LINFO("Module load successful");
    LINFO("No conflicts with other modules detected");
    
	if (tobealigned_.isSelected("Vol1ToVol2") || tobealigned_.isSelected("Vol2ToVol1"))
	{
		Volume* firstVolume;
		Volume* secondVolume;
		
		if (tobealigned_.isSelected("Vol1ToVol2")) {
		    firstVolume  = dynamic_cast<Volume*>(volumes->at(0));
		    secondVolume = dynamic_cast<Volume*>(volumes->at(1));
		}
		
		if (tobealigned_.isSelected("Vol2ToVol1")) {
		    firstVolume  = dynamic_cast<Volume*>(volumes->at(1));
		    secondVolume = dynamic_cast<Volume*>(volumes->at(0));
		}
		
		if (firstVolume == 0 || secondVolume == 0) {
		    LWARNING("Some of the input volumes is 0");
		    return;
		}
		
		tgt::Matrix4d norm1 = GetAlignment(firstVolume);
		tgt::Matrix4d norm2 = GetAlignment(secondVolume);

        tgt::Matrix4d inv2;
		norm2.invert(inv2);
		
		tgt::Matrix4d wrld1 = firstVolume->getPhysicalToWorldMatrix();
		
		tgt::Matrix4d newMatrix = inv2*norm1*wrld1;
        
        if (createNew_.get() == true) {
		    Volume* combinedVolume = firstVolume->clone();
		    combinedVolume->setPhysicalToWorldMatrix(newMatrix);
            
            std::string url1 = firstVolume->getOrigin().getURL();
            std::string url2 = secondVolume->getOrigin().getFilename();
            size_t dotPos = url1.find_last_of('.');
            url1 = url1.substr(0, dotPos);
            std::string newOrigin = url1 + "_align_to_" + url2;
            combinedVolume->setOrigin(VolumeURL(newOrigin));
            
            std::stringstream info;
            info << "Output origin: " << newOrigin;
            LINFO(info.str());
            
		    getSourceProcessor()->addVolume(combinedVolume, true, true);
		}
		else {
		    firstVolume->setPhysicalToWorldMatrix(newMatrix);
		    getSourceProcessor()->invalidateOutport();
		}
	}

	if (tobealigned_.isSelected("SelectedToOrigin"))
	for (size_t i = 0; i < volumes->size(); i++)
	{
		Volume* volume = dynamic_cast<Volume*>(volumes->at(i));
		    
		if (volume == 0) {
		    LWARNING("Input volume is 0");
		    return;
		}
 	    
		tgt::Matrix4d norm1 = GetAlignment(volume);

		tgt::Matrix4d wrld1 = volume->getPhysicalToWorldMatrix();
		
		tgt::Matrix4d newMatrix = norm1*wrld1;
        
        if (createNew_.get() == true) {
            Volume* combinedVolume = volume->clone();
		    combinedVolume->setPhysicalToWorldMatrix(newMatrix);

            std::string url = volume->getOrigin().getURL();
            size_t dotPos = url.find_last_of('.');
            std::string ext = url.substr(dotPos+1, 4);
            url = url.substr(0, dotPos);

            combinedVolume->setOrigin(VolumeURL(url + "_align_to_origin" + ext));
		    getSourceProcessor()->addVolume(combinedVolume, true, true);
		}
		else {
		    volume->setPhysicalToWorldMatrix(newMatrix);
		    getSourceProcessor()->invalidateOutport();
		}
	}
	LINFO("Module processing successfully complete");
}

tgt::Matrix4d VolVolAlign :: GetAlignment(const Volume* vol)
{
	PointCloud cloud;

	cloud.VolumeFill(vol);
	tgt::Matrix4d result = cloud.GetShift();
	result = cloud.GetAxes()*result;
	return result;
}


