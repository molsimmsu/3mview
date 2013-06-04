#include "transferfunctioneditor.h"

const std::string TransferFunctionEditor::loggerCat_ = "3MTK.TransferFunctionEditor";

TransferFunctionEditor::TransferFunctionEditor()
  : TFList_("TFList", "Transfer Function List", std::vector<std::string>())

{
    addProperty(TFList_);
}

void TransferFunctionEditor::updateSelection() {
    DensityMapCoProcessor::updateSelection();
    const VolumeCollection* collection = getInputVolumeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    TFList_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        TFList_.addVolume(collection->at(i));
}

/*void TransferFunctionEditor::updateSelection() {
    DensityMapCoProcessor::updateSelection();
    const VolumeCollection* collection = getInputVolumeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    
    const std::vector<Property*>& prop = getProperties();
    std::stringstream str;
    str << "NumProperties: " << prop.size() << ". Collection size:" << collection->size();
    LINFO(str.str());
    
    if (prop.size() > 0)
    for (size_t i = prop.size(); i > 0 ; i--) {
        LINFO("Remove property:");
        
        Property* p = prop[i-1];
        
        LINFO(p->getGuiName());
        LINFO("Removing property:");
        removeProperty(p);
        LINFO("Deleting property:");
        delete p;
        LINFO("OK");
    }
    
    for (size_t i = 0; i < collection->size(); i++) {
        LINFO("Found volume");
        const VolumeBase* vol = collection->at(i);
        if (typeid(*vol) == typeid(const MoleculeVolume)) {
            LINFO("Add transfuncprop:");
            TransFunc* tf = dynamic_cast<const MoleculeVolume*>(vol)->getTransFunc();
            
            std::string id = vol->getOrigin().getFilename();
            LINFO(id);
            
            TransFuncProperty* prop = new TransFuncProperty(id, id);
            prop->set(tf);
            addProperty(prop);
            
            LINFO("OK");
        }
    }
    
    const std::vector<Property*>& prop1 = getProperties();
    std::stringstream str1;
    str1 << "NUmProperties: " << prop1.size();
    LINFO(str1.str());
    for (size_t i = 0; i < prop1.size(); i++) {
        LINFO(prop1[i]->getGuiName());
    }
}*/

void TransferFunctionEditor::invalidate(int inv) {
    LINFO("Invalidate");
    getSourceProcessor()->getPort("volumecollection")->invalidatePort();
}
