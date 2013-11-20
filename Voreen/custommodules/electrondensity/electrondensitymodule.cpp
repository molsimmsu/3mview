#include "electrondensitymodule.h"

#include "processors/addvolumetocollection.h"
#include "processors/checklistvolumeselector.h"
#include "processors/correlation.h"
#include "processors/densitymapcollectionsource.h"
#include "processors/densitymapmanipulation.h"
#include "processors/densitymapraycaster.h"
#include "processors/densitymapsaver.h"
#include "processors/multivolumeselector.h"
#include "processors/pdbtoedm.h"
#include "processors/segmentation.h"
#include "processors/segmentationsplit.h"
#include "processors/transferfunctioneditor.h"
#include "processors/score.h"

#include "io/mrcvolumereader.h"
#include "io/dxvolumereader.h"
using namespace voreen;

ElectronDensityModule::ElectronDensityModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Electron Density Maps");

    registerProcessor(new AddVolumeToCollection());
    registerProcessor(new ChecklistVolumeSelector());
    registerProcessor(new Correlation());
    registerProcessor(new DensityMapCollectionSource());
    registerProcessor(new DensityMapManipulation());
    registerProcessor(new DensityMapRaycaster());
    registerProcessor(new DensityMapSaver());
    registerProcessor(new MultiVolumeSelector());
    registerProcessor(new PDBtoEDM());
    registerProcessor(new SegmentationProcessor());
    registerProcessor(new SegmentationSplit());
    registerProcessor(new TransferFunctionEditor());
    //registerProcessor(new Score());
    
    registerVolumeReader(new MRCVolumeReader());
    registerVolumeReader(new DXVolumeReader());
}
