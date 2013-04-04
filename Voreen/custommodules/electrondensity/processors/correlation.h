#ifndef VRN_Correlation_H
#define VRN_Correlation_H

#include "../../geometry/processors/manipulationbase.h"
#include "../../molecule/ports/moleculeport.h"
#include "densitymapcoprocessor.h"

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

using std::string;
namespace voreen {

class Volume;

class Correlation : public DensityMapCoProcessor{

public:

    static const std::string loggerCat_;

    Correlation();
    virtual ~Correlation();
    virtual Processor* create() const;
    virtual std::string getClassName() const  { return "GaussFiltering"; }
    virtual std::string getCategory() const   { return "Volume blur";                  }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE;        }
protected:

//record of atom types in input PDB with radial electron density distribution
    virtual void setDescriptions() {setDescription("Single volume blur. TODO: correlation");}
    virtual void process();
    virtual void updateSelection();
    Volume* CalculateBlur();
    void OnClick();

    IntProperty resol_;
    ButtonProperty calcblur_; //click this button to generate volume
    VolumeURLListProperty volumeURLList_;
};

} // namespace

#endif
