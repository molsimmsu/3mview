#ifndef VRN_DATABASEMATCH_H
#define VRN_DATABASEMATCH_H

#include "../../molecule/ports/moleculeport.h"
#include "../ports/matrix4doubleport.h"

#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/processors/processor.h"
using namespace voreen;

class DatabaseMatch : public Processor {
public:
    DatabaseMatch();

    // virtual constructor
    virtual Processor* create() const { return new DatabaseMatch(); }

    // documentary functions
    virtual std::string getClassName() const { return "DatabaseMatch";         }
    virtual std::string getCategory()  const { return "Form Analysis";         }
    virtual CodeState getCodeState()   const { return CODE_STATE_EXPERIMENTAL; }



protected:
    virtual void setDescriptions() {
        setDescription("Accepts Molecule or Electron density map as input, outputs a set of PDB-codes [each has char[8] type]");
    }

    virtual void process();

private:
    double *coords;

    double  O[3]; 
    double Ox[3];
    double Oy[3];
    double Oz[3];
    double polynom[3];
    double moments[205];
    double total_weight;
    int    entries;
    int    mom_total;

    VolumePort    		  volinport_;
    MoleculePort           molinport_;  
    StringOptionProperty   sourceselection_;  

    double CalculateMoment(int, int, int);
    double CalculateFourrier(int, int, int);
 //   void   FillOutport();    		
    void   PDBFindAxes();
    double PolynomVal(double);
    void   GetMoments();  
};

#endif // VRN_DATABASEMATCH_H
