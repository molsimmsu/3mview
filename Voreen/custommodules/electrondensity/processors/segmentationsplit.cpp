#include "segmentationsplit.h"

const std::string SegmentationSplit::loggerCat_("3MTK.DensityMap.SegmentationSplit");

SegmentationSplit::SegmentationSplit()
    : volumePort_(Port::INPORT, "volumePort", "Volume Inport")
    , segmentationPort_(Port::INPORT, "segmentationPort", "Segmentation Inport")
    , splitButton_("splitButton", "Split segments")
{
    addPort(volumePort_);
    addPort(segmentationPort_);
    
    addProperty(splitButton_);
    
    splitButton_.onChange(CallMemberAction<SegmentationSplit>(this, &SegmentationSplit::splitSegmentation));
}

void SegmentationSplit::splitSegmentation() {
    LINFO("splitSegmentation() start");
    
	if (volumePort_.getData() == 0) {
		LWARNING("Inport data is 0");
		return;
	}
	
	if (segmentationPort_.getData() == 0) {
		LWARNING("Outport data is 0. You should find seeds first.");
		return;
	}

    const VolumeBase* volume = volumePort_.getData();
    const VolumeBase* segmentation = segmentationPort_.getData();
    
    LINFO("Got input volumes");
    
	const VolumeRAM* volData = volume->getRepresentation<VolumeRAM>();
	const SegVolumeRAM* segData = static_cast<const SegVolumeRAM*>(segmentation->getRepresentation<VolumeRAM>());
	
	LINFO("Got input volumes data");
	
	svec3 dim = volData->getDimensions();
	
	std::stringstream info;
	info << "Volume dimensions " << dim << std::endl;
	LINFO(info.str());
	
    svec3 v;
    std::vector<SubVolume> subVolumes;
    
	FOR_EACH_VOXEL(v, svec3(0,0,0), dim)
    {
        int segID = segData->voxel(v);
        if (segID == 0) continue;
        
		while (segID > subVolumes.size())
		    subVolumes.push_back(SubVolume(svec3(10000, 10000, 10000), svec3(0,0,0)));
		
		subVolumes[segID-1].expand(v);
	}
	
	LINFO("Got subvolumes bounding boxes");
	
	std::vector<Volume*> volumes;
	vec3 spacing = volume->getSpacing();
	vec3 offset = volume->getOffset();
	tgt::mat4 transform = volume->getPhysicalToWorldMatrix();
	
	for (size_t i = 0; i < subVolumes.size(); i++) {
	    svec3 min = subVolumes[i].getMin();
	    svec3 max = subVolumes[i].getMax();
	    svec3 dimensions =	max - min + svec3(1,1,1);
	    
	    vec3 partOffset(min[0]*spacing[0],min[1]*spacing[1],min[2]*spacing[2]);
	    
	    std::stringstream info;
	    info << "Creating subvolume " << min << " " << max << " " << dimensions << std::endl;
	    LINFO(info.str());
	    
	    VolumeRAM* partData = volData->createNew(dimensions, true);
	    
	    FOR_EACH_VOXEL(v, svec3(0,0,0), dimensions)
        {
            float value = 0;
            int segID = segData->voxel(v + min);
            
            if (segID == i+1)
                value = volData->getVoxelNormalized(v + min);
            
            partData->setVoxelNormalized(value, v);
        }
	    
	    try {
	        std::string baseURL = volume->getOrigin().getURL();
	        
	        std::stringstream url;
	        url << baseURL << "_seg" << i+1;
	    
	        Volume* volume = new MoleculeVolume(partData, spacing, offset + partOffset, transform);
	        volume->setOrigin(VolumeURL(url.str()));
	        
	        getSourceProcessor()->addVolume(volume, true, false);
	    }
	    catch(...) {
	        LERROR("Error at new Volume(). Stop"); return;
	    }
	}
	
	LINFO("splitSegmentation() finished");
}
