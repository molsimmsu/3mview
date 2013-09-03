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

void TransferFunctionEditor::invalidate(int inv) {
    getSourceProcessor()->invalidateOutport();
}
