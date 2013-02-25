#ifndef VRN_VolumeSelectionProperty_H
#define VRN_VolumeSelectionProperty_H

#include "voreen/core/properties/templateproperty.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API TemplateProperty<const VolumeCollection*>;
#endif


class VRN_CORE_API VolumeSelectionProperty : public TemplateProperty<const VolumeCollection*> {
public:
    static std::string loggerCat_;

    VolumeSelectionProperty(const std::string& id, const std::string& guiText, const VolumeCollection* value = 0,
        int invalidationLevel=Processor::INVALID_RESULT);
    VolumeSelectionProperty();
    virtual ~VolumeSelectionProperty() {}

    virtual Property* create() const;

    virtual std::string getClassName() const       { return "VolumeSelectionProperty"; }
    virtual std::string getTypeDescription() const { return "VolumeCollection"; }
    
    /*
     * Return a collection of input volumes
     */
    const VolumeCollection* getInputVolumes() const;
    
    /*
     * Set a collection of input volumes
     */
    void setInputVolumes(const VolumeCollection*);
    /*
     * Return a collection of selected volumes
     */
    VolumeCollection* getSelectedVolumes() const;
    
    /*
     * Set a collection of selected volumes
     */
    void setSelectedVolumes(VolumeCollection*);
    
    /*
     * Checks if the given volume is selected
     */
    bool isSelected(const VolumeBase*) const;
    
    /*
     * Set the selection flag for a given volume
     */
    void setSelected(VolumeBase* volumeHandle, bool selected);
    
    /*
     * Set the selection flag for all volumes
     */
    void setAllSelected(bool selected);

private:
    const VolumeCollection* inputVolumes_; ///< Volume collection containing all input volumes
    VolumeCollection* selectedVolumes_; ///< Volume collection containing selected volumes
    
};

}

#endif

