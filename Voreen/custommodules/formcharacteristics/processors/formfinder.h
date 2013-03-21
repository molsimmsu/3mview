#ifndef VRN_FORMFINDER_H
#define VRN_FORMFINDER_H

#define   SCALE		    	30
#define	SOLVE_ITER		52
#define	PI_2	  			1.57079632679
#define	MAX_SIZE			1.5
#define	DB_FILENAME		"../../DomainDB/mdb.dat"
#define	NAMELEN			8
#define	MAX_ORDER			8


#include "../../molecule/processors/moleculecoprocessor.h"
#include "../../homology/properties/alignmentlistproperty.h"

#include "voreen/core/properties/callmemberaction.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/intproperty.h"
using namespace voreen;

#include "tgt/filesystem.h"

#include <cmath>
#include <string>

class FormFinder : public MoleculeCoProcessor {
public:
    FormFinder();
    
    virtual std::string getClassName() const { return "FormFinder"; }
    virtual std::string getCategory() const  { return "Database"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; }
    
    virtual Processor* create() const { return new FormFinder(); }
    
    virtual void process() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Find homologous domains from BLAST database for a given volume");
    }
    
    /*
     * Find domains matching a given sequence and put them
     * into the AlignmentListProperty
     */
    void findDomains();

    
private:
    ButtonProperty        findDomains_;
    AlignmentListProperty alignmentList_;
    IntProperty           maxDomainsToLoad_;
	
    VolumePort volinport_;    



    float  *coords;

    double  O[3]; 
    double Ox[3];
    double Oy[3];
    double Oz[3];
    double polynom[3];
    double moments[240];
    double total_weight;
    int    entries;
    int    mom_total; 

    double CalculateMoment(int, int, int);
    double CalculateFourrier(int, int, int); 		
    void   FindAxes();
    double PolynomVal(double);
    void   GetMoments();  

    static std::string loggerCat_;
};

#endif