#ifndef VRN_ALIGNBYMOMENTS_H
#define VRN_ALIGNBYMOMENTS_H

#include "../ports/weightedpointcloudport.h"
#include "../../molecule/ports/moleculeport.h"
#include "../ports/matrix4doubleport.h"

#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/processors/processor.h"
using namespace voreen;

class AlignByMoments : public Processor {
public:
    AlignByMoments();

    // virtual constructor
    virtual Processor* create() const { return new AlignByMoments(); }

    // documentary functions
    virtual std::string getClassName() const { return "AlignByMoments";        }
    virtual std::string getCategory()  const { return "Form Analysis";         }
    virtual CodeState getCodeState()   const { return CODE_STATE_EXPERIMENTAL; }

protected:
    virtual void setDescriptions() {
        setDescription("Accepts WeigthedPointCloud as input, outputs a matrix as a tgt::Matrix4<double>");
    }

    virtual void process();

private:
    double  O[3]; 
    double Ox[3];
    double Oy[3];
    double Oz[3];
    double polynom[3];
    double total_weight;
    int    entries;
    double *coords;

    VolumePort    		  volinport_;
    MoleculePort           molinport_;
    Matrix4doublePort 	  outport_;   
    StringOptionProperty   sourceselection_;  


    double CalculateMoment(int, int, int);
    double CalculateFourrier(int, int, int);
    void   PDBFillOutport();  		
    void   PDBFindAxes();
    double PolynomVal(double);
   
};

#endif // VRN_ALIGNBYMOMENTS_H
