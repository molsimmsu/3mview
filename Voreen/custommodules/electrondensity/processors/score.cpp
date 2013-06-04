#include "score.h"

#include "voreen/core/datastructures/volume/volumeatomic.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"
#include "voreen/core/datastructures/volume/operators/volumeoperatorresize.h"
#include "voreen/core/datastructures/geometry/meshlistgeometry.h"

using tgt::ivec3;
using tgt::svec3;
using tgt::vec3;

namespace voreen {

const std::string Score::loggerCat_("3mview.Score");

Score::Score()
    : distanceType_("measure", "Measure")
    , filteringMode_("filteringMode", "Filtering")
    , output_("scoretext", "Current Score", "")
    , calcButton_("calcButton", "Calculate score")
    , inport1_(Port::INPORT, "inport1", "Volume1")
    , inport2_(Port::INPORT, "inport2", "Volume2")
{
    
    addPort(inport1_);
    addPort(inport2_);

    distanceType_.addOption("l1", "|X|+|Y|");
    distanceType_.addOption("l2", "SQRT(X^2+Y^2)");
    addProperty(distanceType_);

    filteringMode_.addOption("nearest", "Nearest");
    filteringMode_.addOption("linear",  "Linear ");
    filteringMode_.addOption("cubic",   "Cubic");
    filteringMode_.set("linear");
    addProperty(filteringMode_);

    addProperty(calcButton_);
    addProperty(output_);

    calcButton_.onChange(CallMemberAction<Score>(this, &Score::calcAndShowScore));
}

Score::~Score() {}

Processor* Score::create() const {
    return new Score();
}

void Score::calcAndShowScore() {
        const VolumeBase* firstVolume = inport1_.getData();
        const VolumeBase* secondVolume = inport2_.getData();
        
        double result;

        // Optimization loop
        //while (!opt.stop()) {
            clock_t time = clock();
            LINFO("Running module Score");
            
            // Get basic matrix
            tgt::mat4 transformationMatrix = secondVolume->getPhysicalToWorldMatrix();
            
            // Rotate and translate matrix using optimization parameters
            // ... Place your code here - get relative matrix
            //transformationMatrix = relativeMatrix * transformationMatrix;
            
            const VolumeBase* newVolume = new VolumeDecoratorReplaceTransformation(secondVolume, transformationMatrix);
            
            result = calculateScore(firstVolume, newVolume);
		//}

        // Show result		
        std::ostringstream result;
		result << result_d;
		const std::string out = result.str();
		output_.set(out);
		
		std::ostringstream message;
		message << "Module Score successfully finished in " <<  ((double)clock() - time)/CLOCKS_PER_SEC << " seconds";
		LINFO(message.str().c_str());
}

double Score::calculateScore(const VolumeBase* firstVolume, const VolumeBase* secondVolume) {

	if (firstVolume == 0 || secondVolume == 0) {
	    LWARNING("Some of the input volumes is 0");
	    return 100000;
	}

    Volume* combinedVolume = 0;

    if (firstVolume->getNumChannels() == secondVolume->getNumChannels()) {
        LINFO("Performing channel-wise combination.");
    }
    else
    {
        LINFO("Number of channels of input volumes do not match. Combining each channel of first volume with channel 0 of second volume");
    }


    // optimized combination for volumes that share a common grid in world-space
    if (firstVolume->getDimensions() == secondVolume->getDimensions() &&
        firstVolume->getSpacing() == secondVolume->getSpacing()     &&
        firstVolume->getOffset() == secondVolume->getOffset()     &&
        firstVolume->getPhysicalToWorldMatrix() == secondVolume->getPhysicalToWorldMatrix()) {

        try {
            combinedVolume = firstVolume->clone();
        }
        catch (const std::bad_alloc&) {
            LERROR("Failed to create combined volume with dimensions " << firstVolume->getDimensions()
                << " : bad allocation");
        }

        if (combinedVolume) {
            LINFO("Performing optimized combination on common grid with dimensions "
                << firstVolume->getDimensions() << "...");
            combineVolumesOnCommonGrid(combinedVolume, firstVolume, secondVolume);
        }
    }
    // standard combination with resampling
    else {
        combinedVolume = createCombinedVolume(firstVolume, secondVolume);
        if (combinedVolume) {
            LINFO("Creating combined volume with dimensions " << combinedVolume->getDimensions() << " using "
                << filteringMode_.get() << " filtering ...");
            combineVolumes(combinedVolume, firstVolume, secondVolume);
        }
    }

    // CALCULATING SCORE
    float meanres, temp, temp1;
    int notnull = 0;
    tgt::svec3 dims = combinedVolume->getDimensions();
    tgt::vec3  space  = combinedVolume->getSpacing();
    
    if (combinedVolume)
	{
		double result_d=0; // THIS WILL BE THE RESULT
		clock_t time = clock();
		const VolumeRAM* vr = combinedVolume->getRepresentation<VolumeRAM>();

		if (distanceType_.isSelected("l2"))
		{
			for (int i=0; i<dims.x; ++i)
					for (int j=0; j<dims.y; ++j)
						for (int k=0; k<dims.z; ++k)
						{
							temp=abs(vr->getVoxelNormalized(i, j, k));

							result_d += (temp*temp);


						}
            temp1=sqrt(result_d);
			result_d *= space[0]*space[1]*space[2];
			result_d = sqrt(result_d);
		}

		if (distanceType_.isSelected("l1"))
		{
			for (int i=0; i<dims.x; ++i)
					for (int j=0; j<dims.y; ++j)
						for (int k=0; k<dims.z; ++k)
						{
						    temp=abs(vr->getVoxelNormalized(i, j, k));
							result_d += fabs(vr->getVoxelNormalized(i, j, k));

						}

						 temp1=result_d;
			result_d *= space[0]*space[1]*space[2];
		}

		meanres=temp1;

        return result_d;
	}
	
	return 100000;

}

inline bool withinRange(const tgt::vec3& pos, const tgt::vec3& llf, const tgt::vec3& urb) {
    return tgt::hand(tgt::greaterThanEqual(pos, llf)) &&
           tgt::hand(tgt::lessThanEqual(   pos, urb));
}

tgt::mat4 Score::computeConversionMatrix(const VolumeBase* originVolume, const VolumeBase* destinationVolume) const {
    if (originVolume == destinationVolume)
        return tgt::mat4::identity;
    else {
        tgt::mat4 voxelToWorldOrigin = originVolume->getVoxelToWorldMatrix();
        tgt::mat4 worldToVoxelDestination = destinationVolume->getWorldToVoxelMatrix();
        return worldToVoxelDestination * voxelToWorldOrigin;
    }
}

void Score::combineVolumes(Volume* combinedVolume, const VolumeBase* firstVolume,
                                   const VolumeBase* secondVolume) const {

    tgtAssert(combinedVolume && firstVolume && secondVolume, "Null pointer passed");

    // compute transformation from voxel coordinates of combined volume
    // to voxel coords of input volumes
    tgt::mat4 combinedToFirst = computeConversionMatrix(combinedVolume, firstVolume);
    tgt::mat4 combinedToSecond = computeConversionMatrix(combinedVolume, secondVolume);

    LDEBUG("Voxel-to-world (First): " << combinedToFirst);
    LDEBUG("Voxel-to-world (Second) " << combinedToSecond);

    // determine which volume is the other (non reference) one
    const VolumeBase* otherVolume = secondVolume;

    //
    // voxel-wise combination
    //
    tgt::vec3 dimFirst(firstVolume->getDimensions() - svec3(1));
    tgt::vec3 dimSecond(secondVolume->getDimensions() - svec3(1));

    const bool nearestFiltering = filteringMode_.isSelected("nearest");
    const bool linearFiltering = filteringMode_.isSelected("linear");
    const bool cubicFiltering = filteringMode_.isSelected("cubic");

    const VolumeRAM* v1 = firstVolume->getRepresentation<VolumeRAM>();
    const VolumeRAM* v2 = secondVolume->getRepresentation<VolumeRAM>();
    VolumeRAM* vc = combinedVolume->getWritableRepresentation<VolumeRAM>();

    VRN_FOR_EACH_VOXEL_WITH_PROGRESS(pos, tgt::ivec3(0), combinedVolume->getDimensions(), progressBar_) {
        for (size_t referenceChannel = 0; referenceChannel < combinedVolume->getNumChannels(); ++referenceChannel) {
            size_t otherChannel = (combinedVolume->getNumChannels() == otherVolume->getNumChannels()) ? referenceChannel : 0;

            // transform sampling pos to coordinate systems of input volumes
            tgt::vec3 posFirst = combinedToFirst*tgt::vec3(pos);
            tgt::vec3 posSecond = combinedToSecond*tgt::vec3(pos);

            // sample input volumes, if transformed voxel position lies inside respective volume
            float valFirst = 0.f;
            float valSecond = 0.f;
            if (nearestFiltering) {
                if (withinRange(posFirst, tgt::vec3::zero, dimFirst))
                    valFirst = v1->getVoxelNormalized(tgt::iround(posFirst), referenceChannel);
                if (withinRange(posSecond, tgt::vec3::zero, dimSecond))
                    valSecond = v2->getVoxelNormalized(tgt::iround(posSecond), otherChannel);
            }
            else if (linearFiltering) {
                if (withinRange(posFirst, tgt::vec3::zero, dimFirst))
                    valFirst = v1->getVoxelNormalizedLinear(posFirst, referenceChannel);
                if (withinRange(posSecond, tgt::vec3::zero, dimSecond))
                    valSecond = v2->getVoxelNormalizedLinear(posSecond, otherChannel);

            }
            else if (cubicFiltering) {
                if (withinRange(posFirst, tgt::vec3::zero, dimFirst))
                    valFirst = v1->getVoxelNormalizedCubic(posFirst, referenceChannel);
                if (withinRange(posSecond, tgt::vec3::zero, dimSecond))
                    valSecond = v2->getVoxelNormalizedCubic(posSecond, otherChannel);
            }
            else {
                LERROR("Unknown filter mode: " << filteringMode_.get());
                return;
            }

            // apply operation to sampled values (note: a switch-block within a volume traversal loop
            // should normally be avoided, however in this case the main operations are way more expensive)
            float result = 0.f;
            result = valFirst - valSecond;
            // assign clamped result to combined volume
            vc->setVoxelNormalized(tgt::clamp(result, 0.f, 1.f), pos, referenceChannel);
        }
    } // VRN_FOR_EACH_VOXEL_WITH_PROGRESS
}

void Score::combineVolumesOnCommonGrid(Volume* combinedVolume, const VolumeBase* firstVolume,
                                               const VolumeBase* secondVolume) const {
    tgtAssert(combinedVolume && firstVolume && secondVolume, "Null pointer passed");
    tgtAssert(combinedVolume->getDimensions() == firstVolume->getDimensions() &&
              combinedVolume->getDimensions() == secondVolume->getDimensions(), "Volume dimensions mismatch");

    // determine which volume is the other (non reference) one
    const VolumeBase* otherVolume = secondVolume;

    const VolumeRAM* v1 = firstVolume->getRepresentation<VolumeRAM>();
    const VolumeRAM* v2 = secondVolume->getRepresentation<VolumeRAM>();
    VolumeRAM* vc = combinedVolume->getWritableRepresentation<VolumeRAM>();

    VRN_FOR_EACH_VOXEL_WITH_PROGRESS(pos, tgt::ivec3(0), combinedVolume->getDimensions(), progressBar_) {
        for (size_t referenceChannel = 0; referenceChannel < combinedVolume->getNumChannels(); ++referenceChannel) {
            size_t otherChannel = (combinedVolume->getNumChannels() == otherVolume->getNumChannels()) ? referenceChannel : 0;
            float valFirst = v1->getVoxelNormalized(pos, referenceChannel);
            float valSecond = v2->getVoxelNormalized(pos, otherChannel);

            // apply operation to input voxel values
            float result = 0.f;
            result = valFirst - valSecond;
            // assign clamped result to combined volume
            //vc->setVoxelNormalized(tgt::clamp(result, 0.f, 1.f), pos, referenceChannel); //FIXME: clamp prevents working with float volumes
            vc->setVoxelNormalized(result, pos, referenceChannel); //FIXME: clamp prevents working with float volumes
        } // for referenceChannel
    } // VRN_FOR_EACH_VOXEL_WITH_PROGRESS
}

Volume* Score::createCombinedVolume(const VolumeBase* refVolume, const VolumeBase* secondVolume) const {
    // compute untransformed bounding box of reference volume
    tgt::vec3 refLLF = refVolume->getLLF();  // all components negative
    tgt::vec3 refURB = refVolume->getURB();  // all components positive
    std::pair<vec3, vec3> refBB = std::pair<vec3, vec3>(refVolume->getLLF(), refVolume->getURB());

    // create bounding box for second volume and transform into reference coordinate system
    MeshGeometry secondBB = MeshGeometry::createCube(secondVolume->getLLF(), secondVolume->getURB());
    tgt::mat4 transformToRef = refVolume->getWorldToPhysicalMatrix();
    transformToRef *= secondVolume->getPhysicalToWorldMatrix();
    secondBB.transform(transformToRef);

    // determine combined bounding box of reference volume and transformed second volume
    tgt::vec3 combinedLLF = refLLF;
    tgt::vec3 combinedURB = refURB;
    for (MeshGeometry::const_iterator face = secondBB.begin(); face != secondBB.end(); ++face) {
        for (FaceGeometry::const_iterator vertex = face->begin(); vertex != face->end(); ++vertex) {
            combinedLLF = tgt::min(combinedLLF, vertex->getCoords());
            combinedURB = tgt::max(combinedURB, vertex->getCoords());
        }
    }
    std::pair<vec3, vec3> combinedBB = std::pair<vec3, vec3>(combinedLLF, combinedURB);
    LDEBUG("Combined BB: " << combinedBB.first << ", " << combinedBB.second);
    tgtAssert(tgt::hand(tgt::lessThanEqual(combinedBB.first, refBB.first)) &&
        tgt::hand(tgt::greaterThanEqual(combinedBB.second, refBB.second)), "Invalid combined bounding box");

    // derive resolution of combined volume from size ratios of combinedBB/refBB and the ref volume's
    // original resolution => spatial resolution of combined volume equals spatial res. of reference volume
    tgt::vec3 scaleFactors = (combinedBB.second - combinedBB.first) / (refBB.second - refBB.first);
    tgtAssert(tgt::hand(tgt::greaterThan(scaleFactors, vec3(0.f))), "invalid scale factors");
    tgt::svec3 combinedDim = svec3(tgt::iceil(vec3(refVolume->getDimensions()) * scaleFactors));
    LDEBUG("Scale factors: " << scaleFactors);
    LDEBUG("Common Dim: " << combinedDim);
    tgtAssert(tgt::hand(tgt::greaterThanEqual(combinedDim, refVolume->getDimensions())),
        "Invalid combined volume dimensions");

    // create combined volume with proper dimensions and spacing
    Volume* combinedVolume = 0;
    try {
        combinedVolume = VolumeOperatorResize::APPLY_OP(refVolume, combinedDim);
    }
    catch (const std::bad_alloc&) {
        LERROR("Failed to create combined volume with dimensions " << combinedDim << " : bad allocation");
        delete combinedVolume;
        return 0;
    }

    // determine correct offset and spacing for combined volume
    combinedVolume->setOffset(combinedBB.first);
    vec3 s = combinedBB.second - combinedBB.first;
    s /= vec3(combinedDim);
    combinedVolume->setSpacing(s);
    combinedVolume->setPhysicalToWorldMatrix(refVolume->getPhysicalToWorldMatrix());

    return combinedVolume;
}

} // namespace
