#ifndef VRN_AddVolumeToCollection_H
#define VRN_AddVolumeToCollection_H

#include "densitymapcoprocessor.h"

#include "voreen/core/ports/volumeport.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/datastructures/volume/volumeram.h"
#include "voreen/core/datastructures/volume/volumedecorator.h"
using namespace voreen;
 
class AddVolumeToCollection : public DensityMapCoProcessor {
public:
    AddVolumeToCollection();
    virtual ~AddVolumeToCollection() {}
    virtual Processor* create() const { return new AddVolumeToCollection(); }

    virtual std::string getClassName() const { return "AddVolumeToCollection";       }
    virtual std::string getCategory() const  { return "Input";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    void process() {}
    
    virtual void updateSelection() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Add an input volume to the density map collection");
    }
    
    void addVolume();
    
    static const std::string loggerCat_;

private:
    VolumePort volumePort_;
    
    ButtonProperty addButton_;

};

#endif // VRN_DENSITYMAPMANIPULATION_H
