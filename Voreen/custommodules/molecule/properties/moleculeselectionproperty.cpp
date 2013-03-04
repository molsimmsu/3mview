#include "moleculeselectionproperty.h"

namespace voreen {

std::string MoleculeSelectionProperty::loggerCat_("MoleculeSelectionProperty");

MoleculeSelectionProperty::MoleculeSelectionProperty(const std::string& id, const std::string& guiText, 
                   const MoleculeCollection* inputMolecules, int invalidationLevel)
    : TemplateProperty<const MoleculeCollection*>(id, guiText, 0, invalidationLevel)
{
    if (!inputMolecules_) inputMolecules_ = new MoleculeCollection();
    selectedMolecules_ = new MoleculeCollection();
}

MoleculeSelectionProperty::MoleculeSelectionProperty() {
    inputMolecules_ = new MoleculeCollection();
    selectedMolecules_ = new MoleculeCollection();
}

Property* MoleculeSelectionProperty::create() const {
    return new MoleculeSelectionProperty();
}

const MoleculeCollection* MoleculeSelectionProperty::getInputMolecules() const {
    return inputMolecules_;
}

void MoleculeSelectionProperty::setInputMolecules(const MoleculeCollection* inputMolecules) {
    inputMolecules_ = inputMolecules;
    updateWidgets();
}

MoleculeCollection* MoleculeSelectionProperty::getSelectedMolecules() const {
    return selectedMolecules_;
}

void MoleculeSelectionProperty::setSelectedMolecules(MoleculeCollection* selectedMolecules) {
    selectedMolecules_ = selectedMolecules;
    updateWidgets();
}
    
bool MoleculeSelectionProperty::isSelected(const Molecule* moleculeHandle) const {
    return selectedMolecules_->contains(moleculeHandle);
}

void MoleculeSelectionProperty::setSelected(Molecule* moleculeHandle, bool selected) {
    if (!getInputMolecules()->contains(moleculeHandle)) {
        LWARNING("setSelected(): passed URL not contained by this property");
        return;
    }
	
	if (!isSelected(moleculeHandle) && selected) {
		selectedMolecules_->add(moleculeHandle);
		invalidate();
	}
	else if (isSelected(moleculeHandle) && !selected) {
		selectedMolecules_->remove(moleculeHandle);
		invalidate();
	}
}

void MoleculeSelectionProperty::setAllSelected(bool selected) {
    const MoleculeCollection* allMolecules = getInputMolecules();
    for (size_t i = 0; i < allMolecules->size(); i++)
        setSelected(allMolecules->at(i), selected);
}

}   // namespace

