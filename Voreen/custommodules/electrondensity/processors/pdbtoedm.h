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

#include <fstream>
#include <iostream>
#include <assert.h>
#include <cmath>

using std::string;
namespace voreen {

class Volume;

class PDBtoEDM : virtual public Processor{

public:

    static const std::string loggerCat_;
    static const int MaxOfTypes=60;
    int NumberAtom,MaxR,Nh;
    float VoxelPerAngstrem;
    int NumberVoxels_x,NumberVoxels_y,NumberVoxels_z,NumberVoxel_Structure;
    float dr,cx,cy,cz,big_size,resol;
    int size_x,size_y,size_z;

struct AtomicED
    {
        std::string AtomName[MaxOfTypes];
        float AtomED[MaxOfTypes][MaxOfTypes];
        float a1[MaxOfTypes],a2[MaxOfTypes],a3[MaxOfTypes],a4[MaxOfTypes];
        float b1[MaxOfTypes],b2[MaxOfTypes],b3[MaxOfTypes],b4[MaxOfTypes],c[MaxOfTypes];
        int NumberTypes;
    };


    PDBtoEDM();
    virtual ~PDBtoEDM();
    virtual Processor* create() const;
    virtual std::string getClassName() const  { return "PDBtoEDM"; }
    virtual std::string getCategory() const   { return "Input";                  }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE;        }
protected:

//record of atom types in input PDB with radial electron density distribution
    virtual void setDescriptions() {setDescription("Loads multiple electron density maps and provides them as VolumeCollection.");}
    virtual void process();
    virtual void ShowGrid();
    void GenerateEDMGrid_ScatteringFactor(const Molecule* InputMoll);
    void GenerateEDMGrid_StructureFactor(const Molecule* InputMoll);
    void CalcElectronNumber(const VolumeRAM* targetDataset);
    void FindAtomTypesInPDB(const OBMol mol, struct AtomicED* sAtomED);
    float CalcElectronDens(struct AtomicED sAtomED, int k, float R);
    void FindBoundingGeometry(const OBMol mol);
    void adjustPropertyVisibility();
    /// The volume port the loaded data set is written to.
    MoleculePort inport_;
    VolumePort outport_;


    IntProperty atoomr_; //calculated distance (A)
    IntProperty deltaatoomr_; //step of calculate (0.1 A)
    IntProperty resolution_; //select resolution for structure factor calculation; (0.1 A)
    //IntProperty gaussvoxel_; //numberof convolutions voxels
    ButtonProperty generategrid_; //click this button to generate volume
    BoolProperty calcelectronnumb_;
    //BoolProperty gaussfiltering_;
    StringOptionProperty calculationmode_;











};

} // namespace

#endif
