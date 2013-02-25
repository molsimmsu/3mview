#ifndef VRN_MOLECULESOURCE_H
#define VRN_MOLECULESOURCE_H

#include "../ports/weightedpointcloudport.h"

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
        setDescription("Accepts WeigthedPointCloud as input, outputs a set of PDB-codes [each has char[8] type]");
    }

    virtual void process();

private:
    double *atoms;

    double  O[3]; 
    double Ox[3];
    double Oy[3];
    double Oz[3];
    double polynom[3];
    double moments[205];
    double total_weight;
    int    entries;
    int    mom_total;

    WeightedPointCloudPort inport_;
 //   WeightedPointCloudPort outport_;   

    double CalculateMoment(int, int, int);
    double CalculateFourrier(int, int, int);
 //   void   FillOutport();    		
    void   FindAxes();
    double PolynomVal(double);
    void   GetMoments();  
};

#endif // VRN_MOLECULESOURCE_H
