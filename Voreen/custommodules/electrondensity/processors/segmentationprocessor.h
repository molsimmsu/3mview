#ifndef VRN_SEGMENTATIONPROCESSOR_H
#define VRN_SEGMENTATIONPROCESSOR_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/volumeport.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"
using namespace voreen;

#include "tgt/vector.h"
using tgt::vec3;
using tgt::svec3;

#include <vector>

typedef seg_t uint8_t;
typedef VolumeAtomic<seg_t> SegVolumeRAM;

#define FOR_EACH_VOXEL(VOXEL, MIN, MAX) \
    for ((VOXEL).z = (MIN).z; (VOXEL).z < (MAX).z; ++(VOXEL).z)\
    for ((VOXEL).y = (MIN).y; (VOXEL).y < (MAX).y; ++(VOXEL).y)\
    for ((VOXEL).x = (MIN).x; (VOXEL).x < (MAX).x; ++(VOXEL).x)

class Segmentation {
public:
	Segmentation(const VolumeRAM* vol, SegVolumeRAM* seg, float to)
		: volume(vol)
		, segmentation(seg)
		, threshold(to)
	{}
	
	const VolumeRAM* volume;
	SegVolumeRAM* segmentation;
	float threshold;
};

class SegmentationProcessor : public Processor {
public:
	SegmentationProcessor();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "SegmentationProcessor";   }
    virtual std::string getCategory() const  { return "Input";            }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL;  }

protected:
    virtual void setDescriptions() {
        setDescription("Performs a simple volume segmntation algorithm by Shaytan, Shurov, Armeev");
    }

    virtual void process() {}
private:
	bool emptyVoxel(Segmentation& seg, svec3 voxel);
	Volume* segmentVolume(const Volume* volume, float threshold);
	void fillSegment(Segmentation& seg, svec3 voxel, int segID);
	
	void runSegmentation();
	
	VolumePort inport_;
	VolumePort outport_;
	FloatProperty threshold_;
	ButtonProperty startButton_;
};

#endif