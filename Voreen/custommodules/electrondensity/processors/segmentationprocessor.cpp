#include "segmentationprocessor.h"

bool SegmentationProcessor::emptyVoxel(Segmentation& seg, svec3 v) {
	float value = seg.volume->getVoxelNormalized(v);
	seg_t segment = seg.segmentation->voxel(v);
	
	if (value > seg.threshold && segment == 0) return true;
	return false;
}

Volume* SegmentationProcessor::segmentVolume(const VolumeBase* volume, float threshold) {
	const VolumeRAM* volData = volume->getRepresentation<VolumeRAM>();
	svec3 dim = volume->getDimensions();
	
	SegVolumeRAM* segData = new SegVolumeRAM(dim);
	Segmentation seg(volData, segData, threshold);
	
	svec3 v; // position of a voxel
	
	// All voxels with value under the threshold will have segmentID = 0
	// XXX Threshold is given normalized 
	for (v[0] = 0; v[0] < dim[0]; v[0]++)
	for (v[1] = 0; v[1] < dim[1]; v[1]++)
	for (v[2] = 0; v[2] < dim[2]; v[2]++)
	//FOR_EACH_VOXEL(v, svec3(0,0,0), dim)
		seg.segmentation->voxel(v) = 0;
	
	seg_t segID = 1;
	
	uint32_t numProcessedVoxels = 0, numTotalVoxels = dim[0] * dim[1] * dim[2];
	
	for (v[0] = 0; v[0] < dim[0]; v[0]++) {
	for (v[1] = 0; v[1] < dim[1]; v[1]++)
	for (v[2] = 0; v[2] < dim[2]; v[2]++, numProcessedVoxels++) 
	//FOR_EACH_VOXEL(v, svec3(0,0,0), dim)
		if (emptyVoxel(seg, v)) {
			fillSegment(seg, v, segID++);
			std::cout << "New segment found: " << (int)segID << std::endl;
			if ((int)segID == 255) {
			    std::cout << "segID == 255. break. " << std::endl;
			    return new Volume(segData, vec3(1, 1, 1), vec3(0, 0, 0));
			}
		}
			
		std::cout << "Progress: " << (float)numProcessedVoxels / numTotalVoxels << std::endl;
	}
	
	std::cout << "Found " << (int)segID << " segments" << std::endl;
			
	return new Volume(segData, vec3(1, 1, 1), vec3(0, 0, 0));
}

void SegmentationProcessor::fillSegment(Segmentation& seg, svec3 voxel, seg_t segID) {
	std::vector<svec3>* front = new std::vector<svec3>();
	front->push_back(voxel);
	
	svec3 dim = seg.volume->getDimensions();
	
	while (front->size() > 0) {
		std::vector<svec3>* newFront = new std::vector<svec3>();
		
		for (size_t i = 0; i < front->size(); i++) {
			svec3 rel; // relative position of each neighbour
			for (rel[0] = 0; rel[0] < 3; rel[0]++)
			for (rel[1] = 0; rel[1] < 3; rel[1]++)
			for (rel[2] = 0; rel[2] < 3; rel[2]++) 
			//FOR_EACH_VOXEL(rel, svec3(-1,-1,-1), svec3(2,2,2))
			{
				svec3 neighbour = front->at(i) + rel - svec3(1,1,1);
				if (neighbour[0] < 0 || neighbour[1] < 0 || neighbour[2] < 0) continue;
				if (neighbour[0] >= dim[0] || neighbour[1] >= dim[1] || neighbour[2] >= dim[2]) continue;
				
				if (emptyVoxel(seg, neighbour)) {
					newFront->push_back(neighbour);
					seg.segmentation->voxel(neighbour) = segID;
					std::cout << "Setting segment ID " << (int)segID << " to voxel" << neighbour << std::endl;
				}
			}
		}
		
		delete front;
		front = newFront;
	}
	delete front;
}

SegmentationProcessor::SegmentationProcessor()
  : inport_(Port::INPORT, "inport", "Volume Inport")
  , outport_(Port::OUTPORT, "outport", "Segmentation Outport")
  , threshold_("threshold", "Normalized Threshold", 0.5, 0.0, 1.0)
  , startButton_("segStart", "Start Segmentation")
{
	addPort(inport_);
	addPort(outport_);
	
	addProperty(threshold_);
	addProperty(startButton_);
	
	startButton_.onChange(CallMemberAction<SegmentationProcessor>(this, &SegmentationProcessor::runSegmentation));
}

Processor* SegmentationProcessor::create() const {
    return new SegmentationProcessor();
}

void SegmentationProcessor::runSegmentation() {
	if (inport_.getData() == 0) {
		LWARNING("Inport data is 0");
		return;
	}
	
	Volume* segmentation;
	segmentation = segmentVolume(inport_.getData(), threshold_.get());
	outport_.setData(segmentation);
}
