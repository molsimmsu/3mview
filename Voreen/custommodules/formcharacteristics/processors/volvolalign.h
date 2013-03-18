#ifndef VRN_VOLVOLALIGN_H
#define VRN_VOLVOLALIGN_H

#include "../../molecule/ports/moleculeport.h"
#include "../ports/mat4port.h"

#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/processors/processor.h"
using namespace voreen;

class VolVolAlign : public Processor {
public:
    VolVolAlign();

    // virtual constructor
    virtual Processor* create() const { return new VolVolAlign(); }

    // documentary functions
    virtual std::string getClassName() const { return "VolVolAlign";        }
    virtual std::string getCategory()  const { return "Form Analysis";         }
    virtual CodeState getCodeState()   const { return CODE_STATE_EXPERIMENTAL; }

protected:
    virtual void setDescriptions() {
        setDescription("Accepts WeigthedPointCloud as input, outputs a matrix as a tgt::Matrix4<double>");
    }

    virtual void process();

private:
    double O[3]; 
    double Ox[3];
    double Oy[3];
    double Oz[3];
    double polynom[3];
    double total_weight;
    size_t entries;
    float  *coords;

    VolumePort      	  volinport1_;
    VolumePort             volinport2_;
    VolumePort             outport_;
    StringOptionProperty   tobealigned_;  


    double CalculateMoment(int, int, int);
    double CalculateFourrier(int, int, int); 		
    void   PDBFindAxes();
    double PolynomVal(double);

    tgt::mat4 GetTransformation(const VolumeBase* vol);

    static const std::string loggerCat_; ///< category used in logging
};

#endif // VRN_VOLVOLALIGN_H