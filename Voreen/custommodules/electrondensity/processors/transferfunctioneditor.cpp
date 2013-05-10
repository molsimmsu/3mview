#include "transferfunctioneditor.h"

TransferFunctionEditor::TransferFunctionEditor()
  //: func1_("func1", "Transfer function 1")

{

}

void TransferFunctionEditor::updateSelection() {
    DensityMapCoProcessor::updateSelection();
    const VolumeCollection* collection = getInputVolumeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    
    const std::vector<Property*>& prop = getProperties();
    for (size_t i = 0; i < prop.size(); i++) {
        removeProperty(prop[i]);
        //delete prop[i];
    }
    
    for (size_t i = 0; i < collection->size(); i++) {
        LINFO("Found volume");
        const VolumeBase* vol = collection->at(i);
        if (typeid(*vol) == typeid(const MoleculeVolume)) {
            TransFunc* tf = dynamic_cast<const MoleculeVolume*>(vol)->getTransFunc();
            
            std::string id = vol->getOrigin().getFilename();
            
            tf_.push_back(TransFuncProperty(id, id));
            tf_.back().set(tf);
            addProperty(tf_.back());
            LINFO("Add transfuncprop:");
            LINFO(id);
        }
    }
}
