#include "calculatescore.h"

CalculateScore :: CalculateScore()
  : Processor(),
    output_("scoretext", "Current Score", ""),
    volinport_(Port::INPORT,   "volume",   "Electon density map")

{
    addProperty(output_);
    addPort(volinport_);
}

void CalculateScore :: process()
{  
	tgt::svec3 dims   = volinport_.getData()->getDimensions();
	tgt::vec3  space  = volinport_.getData()->getSpacing();
     double result_d = 0;
	const VolumeRAM* vr = (volinport_.getData())->getRepresentation<VolumeRAM>();

	for (int i=0; i<dims.x; ++i)
			for (int j=0; j<dims.y; ++j)
				for (int k=0; k<dims.z; ++k)
				{
					result_d += (vr->getVoxelNormalized(i, j, k))*(vr->getVoxelNormalized(i, j, k));
				}
	result_d *= space[0]*space[1]*space[2];
     std::ostringstream result;
	result << result_d;
	const std::string out = result.str();
	output_.set(out);
}
