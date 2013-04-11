#include "qualitymodule.h"
#include "voreen/core/processors/processorwidget.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"


#include <fstream>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <cmath>

#include "tgt/exception.h"
#include "tgt/vector.h"
#include "tgt/texturemanager.h"

#include "voreen/core/datastructures/volume/volumeatomic.h"
#define SHOWSTATDEF    CallMemberAction<QualityModule>(this, &QualityModule::ShowStatistics)
#define ANALYZESTATDEF CallMemberAction<QualityModule>(this, &QualityModule::Analyze)
float FindTime, MomentsNumb, Resolution;
using std::string;

namespace voreen {

const std::string QualityModule::loggerCat_("voreen.core.QualityModule");

QualityModule::QualityModule()
    : Processor()
, ShowStat_("ShowStat", "ShowStat")
, AnalyzeStat_(" AnalyzeStat", " AnalyzeStat")



{
addProperty(ShowStat_);
addProperty(AnalyzeStat_);

ShowStat_.onClick(SHOWSTATDEF);
AnalyzeStat_.onClick(ANALYZESTATDEF);

LINFO("Loaded successfully");
LINFO("No conflicts with modules");
}

QualityModule::~QualityModule() {
}

Processor* QualityModule::create() const {
        return new QualityModule();
}

void QualityModule::process() {}

void QualityModule::ShowStatistics() {
std::cout<<"Seraching time (s): "<<FindTime<<std::endl;
std::cout<<"Moments number : "<<MomentsNumb<<std::endl;
std::cout<<"Resolution (nm): "<<Resolution<<std::endl;
}
void QualityModule::Analyze() {

    if (FindTime<30*60) std::cout<<"Seraching time (s): OK (< 30 minutes)"<<std::endl;
    else std::cout<<"Seraching time (s): INCORRECT (> 30 minutes)"<<std::endl;
    if (MomentsNumb>5) std::cout<<"Moments number: OK (>= 5)"<<std::endl;
    else std::cout<<"Moments number: INCORRECT (< 5)"<<std::endl;
    if (MomentsNumb<=2) std::cout<<"Resolution (nm): OK (<= 2 nm)"<<std::endl;
    else std::cout<<"Resolution (nm): INCORRECT (> 2)"<<std::endl;

}


} // namespace
