#ifndef VRN_DENSITYMAPSAVER_H
#define VRN_DENSITYMAPSAVER_H

#include "densitymapcoprocessor.h"

#include <string>
#include "voreen/core/processors/volumeprocessor.h"

#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/optionproperty.h"

namespace voreen {

class Volume;
class VolumeSerializer;
class VolumeSerializerPopulator;

class DensityMapSaver : public DensityMapCoProcessor {
public:
    DensityMapSaver();
    virtual ~DensityMapSaver();
    virtual Processor* create() const;

    virtual std::string getClassName() const  { return "DensityMapSaver";      }
    virtual std::string getCategory() const   { return "Output";          }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE; }
    virtual bool isEndProcessor() const       { return true;              }

    virtual void saveVolume();
    
    virtual void updateSelection();

protected:
    virtual void setDescriptions() {
        setDescription("Saves selected volumes to disk");
    }

    virtual void process();
    virtual void initialize() throw (tgt::Exception);
    virtual void deinitialize() throw (tgt::Exception);

private:
    std::vector<std::string> constructFilters() const;

    FileDialogProperty folder_;
    StringOptionProperty outputFormat_;
    ButtonProperty saveButton_;
    VolumeURLListProperty volumeURLList_;

    VolumeSerializerPopulator* volSerializerPopulator_;

    static const std::string loggerCat_;
};

}   //namespace

#endif
