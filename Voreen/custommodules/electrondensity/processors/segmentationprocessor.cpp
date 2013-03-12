#include "segmentationprocessor.h"

bool SegmentationProcessor::emptyVoxel(Segmentation& seg, tgt::svec3 v) {
	float value = seg.volume.getVoxelNormalized(v);
	seg_t segment = seg.segmentation.voxel(v);
	
	if (value > seg.threshold && segment == 0) return true;
	return false;
}

Volume* SegmentationProcessor::segmentVolume(const Volume* volume, float threshold) {
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
		seg.segmentation.voxel(v) = 0;
	
	seg_t segID = 1;
	
	for (v[0] = 0; v[0] < dim[0]; v[0]++)
	for (v[1] = 0; v[1] < dim[1]; v[1]++)
	for (v[2] = 0; v[2] < dim[2]; v[2]++)
	//FOR_EACH_VOXEL(v, svec3(0,0,0), dim)
		if (emptyVoxel(segData, v)) 
			fillSegment(segData, v, segID++);
			
	return new Volume(segData, vec3(1, 1, 1), vec3(0, 0, 0));
}

void SegmentationProcessor::fillSegment(Segmentation& seg, svec3 voxel, seg_t segID) {
	std::vector<svec3>* front = new std::vector<svec3>();
	front->push_back(voxel);
	
	while (front->size() > 0) {
		std::vector<svec3>* newFront = new std::vector<svec3>();
		
		for (size_t i = 0; i < wavefrontCoords->size(); i++) {
			svec3 rel; // relative position of each neighbour
			for (rel[0] = -1; rel[0] < 2; rel[0]++)
			for (rel[1] = -1; rel[1] < 2; rel[1]++)
			for (rel[2] = -1; rel[2] < 2; rel[2]++) 
			//FOR_EACH_VOXEL(rel, svec3(-1,-1,-1), svec3(2,2,2))
			{
				svec3 neighbour = front->at(i) + rel;
				
				if (emptyVoxel(segData, neighbour)) {
					newFront->push_back(neighbour);
					seg.segmentation.voxel(neighbour) = segID;
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
  , outport_(Port::OUTPORT, "outport", "Segmentation Outport"),
  , threshold_("threshold", "Normalized Threshold", 0.5, 0.0, 1.0)
  , startButton_("segStart", "Start Segmentation")
{
	addPort(inport_);
	addPort(outport_);
	
	addProperty(threshold_);
	addProperty(startButton_);
	
	startButton_.onChange(CallMemberAction<SegmentationProcessor>(this, &SegmentationProcessor::runSegmentation));
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