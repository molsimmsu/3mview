#ifndef VRN_VOLVOLALIGN_H
#define VRN_VOLVOLALIGN_H

#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/processors/processor.h"

using namespace voreen;

tgt::vec3 getVolumeMassCenter(VolumeBase*);

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

    virtual void process() {}
    
    void align();

private:
    tgt::vec3 O; 
    tgt::vec3 Ox;
    tgt::vec3 Oy;
    tgt::vec3 Oz;
    double polynom[4];
    double total_weight;
    size_t entries;
    int sign;
    double  *coords;

    VolumePort             volinport1_;
    VolumePort             volinport2_;
    VolumePort             outport_;
    StringOptionProperty   tobealigned_;
    ButtonProperty         align_;  


    double CalculateMoment(int, int, int);       
    double PolynomVal(double);

    tgt::Matrix4d GetShift(const Volume* vol);
    tgt::Matrix4d GetAxes();

    static const std::string loggerCat_; ///< category used in logging
};

#endif // VRN_VOLVOLALIGN_H
