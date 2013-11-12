// WARNING: This file is generated and will be overwritten on next cmake run!
#include "voreen/qt/voreenapplicationqt.h"

// include qt module class headers
#include "/home/shurov/voreen-src-4.3-unix/modules/base/basemoduleqt.h"
#include "/home/shurov/voreen-src-4.3-unix/modules/core/coremoduleqt.h"
#include "/home/shurov/voreen-src-4.3-unix/modules/dynamicglsl/dynamicglslmoduleqt.h"
#include "/home/shurov/voreen-src-4.3-unix/modules/plotting/plottingmoduleqt.h"
#include "/home/shurov/voreen-src-4.3-unix/modules/randomwalker/randomwalkermoduleqt.h"
#include "/home/shurov/voreen-src-4.3-unix/modules/stereoscopy/stereoscopymoduleqt.h"

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

    module = new RandomWalkerModuleQt("modules/randomwalker");
    vappQt->registerQtModule(module);

    module = new StereoscopyModuleQt("modules/stereoscopy");
    vappQt->registerQtModule(module);

}

} // namespace
