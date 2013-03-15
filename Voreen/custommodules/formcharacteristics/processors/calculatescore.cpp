#include "calculatescore.h"

const std::string CalculateScore::loggerCat_("3mview.CalculateScore");

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
    LINFO("Running module CalculateScore");
	tgt::svec3 dims   = volinport_.getData()->getDimensions();
	tgt::vec3  space  = volinport_.getData()->getSpacing();
	clock_t time = clock();
     double result_d = 0;
	const VolumeRAM* vr = (volinport_.getData())->getRepresentation<VolumeRAM>();

	for (int i=0; i<dims.x; ++i)
			for (int j=0; j<dims.y; ++j)
				for (int k=0; k<dims.z; ++k)
				{
					result_d += (vr->getVoxelNormalized(i, j, k))*(vr->getVoxelNormalized(i, j, k));
				}
	result_d *= space[0]*space[1]*space[2];
	result_d = sqrt(result_d);    
 // result_d /= dims[0]*dims[1]*dims[2];
    std::ostringstream result;
	result << result_d;
	const std::string out = result.str();
	output_.set(out);
	std::ostringstream message;
	message << "Module CalculateScore successfully finished in " <<  ((double)clock() - time)/CLOCKS_PER_SEC << " seconds";
	LINFO(message.str().c_str());
}
