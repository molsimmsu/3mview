// WARNING: This file is generated and will be overwritten on next cmake run!
#include "voreen/core/voreenapplication.h"

// include core module class headers
#include "/home/class3d/softw/Work/Voreen/modules/advancedlighting/advancedlightingmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/base/basemodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/connexe/connexemodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/core/coremodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/devil/devilmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/dynamicglsl/dynamicglslmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/flowreen/flowreenmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/plotting/plottingmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/pvm/pvmmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/segy/segymodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/staging/stagingmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/volumelabeling/volumelabelingmodule.h"
#include "/home/class3d/softw/Work/Voreen/modules/zip/zipmodule.h"
#include "/home/class3d/softw/Work/Voreen/custommodules/molecule/moleculemodule.h"
#include "/home/class3d/softw/Work/Voreen/custommodules/stereo/stereomodule.h"

namespace voreen {

// instantiate core module classes
void registerAllModules(VoreenApplication* vapp) {
    VoreenModule* module;

    module = new AdvancedLightingModule("modules/advancedlighting");
    vapp->registerModule(module);

    module = new BaseModule("modules/base");
    vapp->registerModule(module);

    module = new ConnexeModule("modules/connexe");
    vapp->registerModule(module);

    module = new CoreModule("modules/core");
    vapp->registerModule(module);

    module = new DevILModule("modules/devil");
    vapp->registerModule(module);

    module = new DynamicGLSLModule("modules/dynamicglsl");
    vapp->registerModule(module);

    module = new FlowreenModule("modules/flowreen");
    vapp->registerModule(module);

    module = new PlottingModule("modules/plotting");
    vapp->registerModule(module);

    module = new PVMModule("modules/pvm");
    vapp->registerModule(module);

    module = new SEGYModule("modules/segy");
    vapp->registerModule(module);

    module = new StagingModule("modules/staging");
    vapp->registerModule(module);

    module = new VolumeLabelingModule("modules/volumelabeling");
    vapp->registerModule(module);

    module = new ZIPModule("modules/zip");
    vapp->registerModule(module);

    module = new MoleculeModule("custommodules/molecule");
    vapp->registerModule(module);

    module = new StereoModule("custommodules/stereo");
    vapp->registerModule(module);

}

} // namespace
