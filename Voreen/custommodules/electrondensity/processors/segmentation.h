#ifndef VRN_SEGMENTATIONPROCESSOR_H
#define VRN_SEGMENTATIONPROCESSOR_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/volumeport.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"
using namespace voreen;

#include "tgt/vector.h"
using tgt::vec3;
using tgt::svec3;

#include <vector>
#include <sstream>

typedef uint8_t seg_t;
typedef VolumeAtomic<seg_t> SegVolumeRAM;
typedef std::vector<svec3> Front;

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
	virtual ~SegmentationProcessor() {}
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "SegmentationProcessor";   }
    virtual std::string getCategory() const  { return "Input";            }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE;  }
    
    virtual void process() {}

protected:
    virtual void setDescriptions() {
        setDescription("Performs a volume segmentation algorithm by Shaytan, Shurov, Armeev");
    }
    
private:
	Volume* findSeeds(const VolumeBase* volume, float threshold);
	Volume* growSeeds(const VolumeBase* volume, float threshold);
	bool emptyVoxel(const Segmentation* seg, svec3 v) const;
	void fillSegment(Segmentation* seg, svec3 voxel, seg_t segID);
	Front* expandFront(Segmentation* seg, const Front* front, seg_t segID);
	
	void startSeeding(); // Find seeds
	void startGrowth();  // Grow from seeds
	
	VolumePort inport_;
	VolumePort outport_;
	
	StringProperty status_;
	FloatProperty seedThreshold_;
	FloatProperty growThreshold_;
	ButtonProperty seedButton_;
	ButtonProperty growButton_;
	
	std::vector<Front> seeds;
	std::vector<Front*> fronts;
};

#endif
