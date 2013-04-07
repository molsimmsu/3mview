#ifndef VRN_FORMFINDER_H
#define VRN_FORMFINDER_H

#define	SOLVE_ITER			52
#define	PI_2					1.57079632679
#define	MAX_SIZE				1.5
#define	DB_FILENAME			"../../DomainDB/bin/DomainsDB/moments.dat"
#define   DOMAIN_LOAD_PATH		"../../DomainDB/bin/DomainsDB/domains/"
#define	NAMELEN				8
#define   DB_ORDER				6


#include "../../molecule/processors/moleculecoprocessor.h"
#include "../../electrondensity/processors/densitymapcoprocessor.h"
#include "../../homology/properties/alignmentlistproperty.h"

#include "voreen/core/properties/callmemberaction.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/volumeurllistproperty.h"

using namespace voreen;

#include "tgt/filesystem.h"
#include "../datastructures/pointcloud.h"

#include <cmath>
#include <ctime>
#include <string>

class FormFinder : virtual public DensityMapCoProcessor, virtual public MoleculeCoProcessor {
public:
    FormFinder();
    
    virtual std::string getClassName() const { return "FormFinder"; }
    virtual std::string getCategory() const  { return "Database"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; }
    
    virtual Processor* create() const { return new FormFinder(); }
    
    virtual void process() {}
    
    virtual void updateSelection();
    
protected:
    virtual void setDescriptions() {
        setDescription("Find homologous domains from BLAST database for a given volume");
    }
    
    /*
     * Find domains matching a given sequence and put them
     * into the AlignmentListProperty
     */
    void findDomainsVol();
    void findDomainsMol();

    
private:

    MoleculeURLListProperty moleculeURLlist_;
    VolumeURLListProperty   volumeURLList_;
    ButtonProperty          findDomains1_;
    ButtonProperty          findDomains2_;
    AlignmentListProperty   alignmentList_;
    IntProperty             maxDomainsToLoad_;
    FloatProperty           accuracy_;
    FloatProperty           weightFactor_;
    FloatProperty           massImportance_;

    double *moments;
    void   GetMoments();  

    static std::string loggerCat_;
};

#endif
