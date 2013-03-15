#include "segmentation.h"

SegmentationProcessor::SegmentationProcessor()
  : inport_(Port::INPORT, "inport", "Volume Inport")
  , outport_(Port::OUTPORT, "outport", "Segmentation Outport")
  , status_("status", "Status")
  , seedThreshold_("seedThreshold", "Seed threshold", 0.0, -10.0, 10.0)
  , seedButton_("seedButton", "Start seeding")
  , growThreshold_("growThreshold", "Grow threshold", 0.0, -10.0, 10.0)
  , growButton_("growButton", "Start growing")
{
	addPort(inport_);
	addPort(outport_);
	
	addProperty(status_);
	addProperty(seedThreshold_);
	addProperty(seedButton_);
	addProperty(growThreshold_);
	addProperty(growButton_);
	
	seedButton_.onChange(CallMemberAction<SegmentationProcessor>(this, &SegmentationProcessor::startSeeding));
	growButton_.onChange(CallMemberAction<SegmentationProcessor>(this, &SegmentationProcessor::startGrowth));
}

bool SegmentationProcessor::emptyVoxel(const Segmentation* seg, svec3 v) const {
	float value = seg->volume->getVoxelNormalized(v);
	int segment = seg->segmentation->voxel(v);
	
	if (value > seg->threshold && segment == 0) return true;
	return false;
}

Volume* SegmentationProcessor::findSeeds(const VolumeBase* volume, float threshold) {
	const VolumeRAM* volData = volume->getRepresentation<VolumeRAM>();
	svec3 dim = volume->getDimensions();
	
	SegVolumeRAM* segData = new SegVolumeRAM(dim);
	Segmentation* seg = new Segmentation(volData, segData, threshold);
	
	svec3 v;
	FOR_EACH_VOXEL(v, svec3(0,0,0), dim)
		seg->segmentation->voxel(v) = 0;
	
	seg_t segID = 1;
	
	uint32_t numProcessedVoxels = 0, numTotalVoxels = dim[0] * dim[1] * dim[2];
	
	seeds.clear();
	
	for (v[0] = 0; v[0] < dim[0]; v[0]++) {
	for (v[1] = 0; v[1] < dim[1]; v[1]++)
	for (v[2] = 0; v[2] < dim[2]; v[2]++, numProcessedVoxels++) 
	//FOR_EACH_VOXEL(v, svec3(0,0,0), dim)
		if (emptyVoxel(seg, v)) {
		    seeds.push_back(Front());
			fillSegment(seg, v, segID++);
			std::cout << "New segment found: " << (int)(segID-1) << std::endl;
			if ((int)segID == 255) {
			    std::cout << "segID == 255. break. " << std::endl;
			    return new Volume(segData, vec3(1, 1, 1), vec3(0, 0, 0));
			}
		}
		std::cout << "Progress: " << (float)numProcessedVoxels / numTotalVoxels << std::endl;
	}
	
	std::stringstream statusText;
	statusText << "Found " << (int)(segID-1) << " segments" << std::endl;
	status_.set(statusText.str());
	
	delete seg;
	return new Volume(segData, vec3(1, 1, 1), vec3(0, 0, 0));
}

Volume* SegmentationProcessor::growSeeds(const VolumeBase* volume, float threshold) {
	const VolumeRAM* volData = volume->getRepresentation<VolumeRAM>();
	svec3 dim = volume->getDimensions();
	
	SegVolumeRAM* segData = new SegVolumeRAM(dim);
	Segmentation* seg = new Segmentation(volData, segData, threshold);
	
	svec3 v;
	FOR_EACH_VOXEL(v, svec3(0,0,0), dim)
		seg->segmentation->voxel(v) = 0;
	
	fronts.clear();
	for (size_t i = 0; i < seeds.size(); i++) {
	    Front* newFront = new Front();
	    newFront->insert(newFront->end(), seeds[i].begin(), seeds[i].end());
	    fronts.push_back(newFront);
	}
	
	while (fronts.size() > 0) {
	    for (size_t i = 0; i < fronts.size(); i++) {
		    Front* newFront = expandFront(seg, fronts[i], i+1);
		    delete fronts[i];
		    fronts[i] = newFront;
	    }
	    
	    // stop when there is not a single front expanded on this iteration
	    size_t numActiveFronts = 0;
	    for (size_t i = 0; i < fronts.size(); i++) 
	        if (fronts[i]->size() > 0) numActiveFronts++;
	        
	    if (numActiveFronts == 0) break;
	}
	
	std::stringstream statusText;
	statusText << "Growth finished" << std::endl;
	status_.set(statusText.str());
	
	delete seg;
	return new Volume(segData, vec3(1, 1, 1), vec3(0, 0, 0));
}

Front* SegmentationProcessor::expandFront(Segmentation* seg, const Front* front, seg_t segID) {
	Front* newFront = new Front();
	
	svec3 dim = seg->volume->getDimensions();
	
	for (size_t i = 0; i < front->size(); i++) {
		svec3 rel;
		FOR_EACH_VOXEL(rel, svec3(0,0,0), svec3(3,3,3)) {
			svec3 neighbour = front->at(i) + rel - svec3(1,1,1);
			if (neighbour[0] < 0 || neighbour[1] < 0 || neighbour[2] < 0) continue;
			if (neighbour[0] >= dim[0] || neighbour[1] >= dim[1] || neighbour[2] >= dim[2]) continue;
			
			if (emptyVoxel(seg, neighbour)) {
				newFront->push_back(neighbour);
				seg->segmentation->voxel(neighbour) = segID;
			}
		}
	}
	
	return newFront;
}

void SegmentationProcessor::fillSegment(Segmentation* seg, svec3 voxel, seg_t segID) {
	Front* front = new Front();
	front->push_back(voxel);
	
	while (front->size() > 0) {
	    Front& seed = seeds[segID-1];
	    seed.insert(seed.end(), front->begin(), front->end());
		Front* newFront = expandFront(seg, front, segID);
		delete front;
		front = newFront;
	}
	delete front;
}

Processor* SegmentationProcessor::create() const {
    return new SegmentationProcessor();
}

void SegmentationProcessor::startSeeding() {
	if (inport_.getData() == 0) {
		LWARNING("Inport data is 0");
		return;
	}
	
	Volume* segmentation = findSeeds(inport_.getData(), seedThreshold_.get());
	outport_.setData(segmentation);
}

void SegmentationProcessor::startGrowth() {
	if (inport_.getData() == 0) {
		LWARNING("Inport data is 0");
		return;
	}
	
	if (outport_.getData() == 0) {
		LWARNING("Outport data is 0. You should find seeds first.");
		return;
	}
	
	Volume* segmentation = growSeeds(inport_.getData(), growThreshold_.get());
	outport_.setData(segmentation);
}
