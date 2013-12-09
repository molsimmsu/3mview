#ifndef VRN_NEIGHBOUR_FINDER_H
#define VRN_NEIGHBOUR_FINDER_H

#include <string>
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/vectorproperty.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/buttonproperty.h"

#include "../ports/tableport.h"

namespace voreen {

class NeighbourFinder : public Processor {
public:
    NeighbourFinder();
    virtual Processor* create() const;

    virtual std::string getCategory() const   { return "Table Processing"; }
    virtual std::string getClassName() const  { return "NeighbourFinder";     }
    virtual CodeState getCodeState() const    { return CODE_STATE_TESTING;   }

protected:
    virtual void setDescriptions() {
        setDescription("Finds neighbours for each row and returns their indices lists");
    }

    virtual void process();

private:
    void spacingChanged(int dim);
    void uniformScalingChanged();

    TablePort inport_;
    TablePort outport_;

    BoolProperty enableProcessing_;
    StringOptionProperty mode_;
    BoolProperty uniformSpacing_;
    FloatProperty spacingX_;
    FloatProperty spacingY_;
    FloatProperty spacingZ_;
    FloatVec3Property spacingDisplay_;
    ButtonProperty start_;

    static const std::string loggerCat_; ///< category used in logging
};

}   //namespace

#endif // VRN_NEIGHBOUR_FINDER_H
