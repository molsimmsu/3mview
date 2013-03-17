#ifndef VRN_PDBtoEDM_H
#define VRN_PDBtoEDM_H

#include "../../geometry/processors/manipulationbase.h"
#include "../../molecule/ports/moleculeport.h"

#include "voreen/core/ports/allports.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/geometryport.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"




namespace voreen {

class Volume;

class PDBtoEDM : virtual public Processor{

public:

    static const std::string loggerCat_;
    static const int MaxOfTypes=20;


    PDBtoEDM();
    virtual ~PDBtoEDM();
    virtual Processor* create() const;
    virtual std::string getClassName() const  { return "PDBtoEDM"; }
    virtual std::string getCategory() const   { return "Input";                  }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE;        }
protected:

//record of atom types in input PDB with radial electron density distribution
    struct AtomicED
    {
        std::string AtomName[MaxOfTypes];
        float AtomED[MaxOfTypes][MaxOfTypes];
        int NumberTypes;
    };

    virtual void setDescriptions() {setDescription("Loads multiple electron density maps and provides them as VolumeCollection.");}
    virtual void process();
    virtual void ShowGrid();
    void GenerateEDMGrid(const Molecule* InputMoll);

    /// The volume port the loaded data set is written to.
    MoleculePort inport_;
    VolumePort outport_;


    IntProperty atoomr_; //calculated distance (A)
    IntProperty deltaatoomr_; //step of calculate (0.1 A)
    ButtonProperty generategrid_; //click this button to generate volume
};

} // namespace

#endif
