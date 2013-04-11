#ifndef VRN_QUALITYMODULE_H
#define VRN_QUALITYMODULE_H

#include "../../electrondensity/processors/densitymapcoprocessor.h"

#include "../../geometry/processors/manipulationbase.h"
#include "../../molecule/ports/moleculeport.h"
#include "../../homology/processors/homologyfinder.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/geometryport.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include <fstream>
#include <iostream>
#include <assert.h>
#include <cmath>
extern float FindTime, MomentsNumb, Resolution;

using std::string;
namespace voreen {
class Volume;

class QualityModule : public DensityMapCoProcessor{

public:

    static const std::string loggerCat_;

    QualityModule();
    virtual ~QualityModule();
    virtual Processor* create() const;
    virtual std::string getClassName() const  { return "QualityModule"; }
    virtual std::string getCategory() const   { return "QualityModuleQualityModule";                  }

protected:

//record of atom types in input PDB with radial electron density distribution
    virtual void setDescriptions() {setDescription("Analyze statistic data");}
    virtual void process();
    void ShowStatistics();
    void Analyze();

    ButtonProperty ShowStat_;
    ButtonProperty AnalyzeStat_;
};

} // namespace

#endif
