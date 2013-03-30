#include "electrondensitymodule.h"

#include "processors/segmentation.h"
#include "processors/segmentationsplit.h"
#include "processors/densitymapmanipulation.h"
#include "processors/densitymapcollectionsource.h"
#include "processors/pdbtoedm.h"
#include "processors/correlation.h"
#include "processors/transferfunctioneditor.h"
#include "processors/multivolumeselector.h"
#include "io/mrcvolumereader.h"
#include "processors/addvolumetocollection.h"
#include "processors/checklistvolumeselector.h"
using namespace voreen;

ElectronDensityModule::ElectronDensityModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Electron Density Maps");

    registerProcessor(new ChecklistVolumeSelector());
    registerProcessor(new AddVolumeToCollection());
    registerProcessor(new SegmentationProcessor());
    registerProcessor(new SegmentationSplit());
    registerProcessor(new DensityMapManipulation());
    registerProcessor(new DensityMapCollectionSource());
    registerProcessor(new PDBtoEDM());
    registerProcessor(new Correlation());
    registerProcessor(new TransferFunctionEditor());
    registerProcessor(new MultiVolumeSelector());
    
    registerVolumeReader(new MRCVolumeReader());
}
