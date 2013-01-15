// WARNING: This file is generated and will be overwritten on next cmake run!
#include "voreen/qt/voreenapplicationqt.h"

// include qt module class headers
#include "/home/class3d/softw/Work/Voreen/modules/base/basemoduleqt.h"
#include "/home/class3d/softw/Work/Voreen/modules/core/coremoduleqt.h"
#include "/home/class3d/softw/Work/Voreen/modules/dynamicglsl/dynamicglslmoduleqt.h"
#include "/home/class3d/softw/Work/Voreen/modules/plotting/plottingmoduleqt.h"

namespace voreen {

// instantiate qt module classes
void registerAllQtModules(VoreenApplicationQt* vappQt) {
    VoreenModuleQt* module;

    module = new BaseModuleQt("modules/base");
    vappQt->registerQtModule(module);

    module = new CoreModuleQt("modules/core");
    vappQt->registerQtModule(module);

    module = new DynamicGLSLModuleQt("modules/dynamicglsl");
    vappQt->registerQtModule(module);

    module = new PlottingModuleQt("modules/plotting");
    vappQt->registerQtModule(module);

}

} // namespace
