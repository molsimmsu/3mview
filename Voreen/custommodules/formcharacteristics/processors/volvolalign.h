#ifndef VRN_VOLVOLALIGN_H
#define VRN_VOLVOLALIGN_H

#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "../datastructures/pointcloud.h"
#include "../../electrondensity/processors/densitymapcoprocessor.h"
#include "voreen/core/properties/volumeurllistproperty.h"
using namespace voreen;

class VolVolAlign : public DensityMapCoProcessor {
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
    
    virtual void updateSelection();

private:
    StringOptionProperty   tobealigned_;
    ButtonProperty         align_;
    VolumeURLListProperty volumeURLList_;

    tgt::Matrix4d GetAlignment(const Volume* vol);

    static const std::string loggerCat_; ///< category used in logging
};

#endif // VRN_VOLVOLALIGN_H
