#include "electrondensitymodule.h"

#include "processors/segmentationprocessor.h"
#include "processors/densitymapmanipulation.h"
#include "processors/densitymapcollectionsource.h"
#include "io/mrcvolumereader.h"
using namespace voreen;

ElectronDensityModule::ElectronDensityModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Electron Density Maps");

    registerProcessor(new SegmentationProcessor());
    registerProcessor(new DensityMapManipulation());
    registerProcessor(new DensityMapCollectionSource());
    
    registerVolumeReader(new MRCVolumeReader());
}
