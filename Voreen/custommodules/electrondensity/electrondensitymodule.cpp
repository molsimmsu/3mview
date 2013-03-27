#include "electrondensitymodule.h"

#include "processors/segmentation.h"
#include "processors/densitymapmanipulation.h"
#include "processors/densitymapcollectionsource.h"
#include "processors/pdbtoedm.h"
#include "processors/transferfunctioneditor.h"
#include "processors/multivolumeselector.h"
#include "io/mrcvolumereader.h"
using namespace voreen;

ElectronDensityModule::ElectronDensityModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Electron Density Maps");

    registerProcessor(new SegmentationProcessor());
    registerProcessor(new DensityMapManipulation());
    registerProcessor(new DensityMapCollectionSource());
    registerProcessor(new PDBtoEDM());
    registerProcessor(new TransferFunctionEditor());
    registerProcessor(new MultiVolumeSelector());
    
    registerVolumeReader(new MRCVolumeReader());
}
