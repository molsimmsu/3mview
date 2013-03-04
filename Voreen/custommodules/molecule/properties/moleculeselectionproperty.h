#ifndef VRN_MoleculeSelectionProperty_H
#define VRN_MoleculeSelectionProperty_H

#include "voreen/core/properties/templateproperty.h"
#include "../datastructures/moleculecollection.h"

namespace voreen {

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API TemplateProperty<const MoleculeCollection*>;
#endif


class VRN_CORE_API MoleculeSelectionProperty : public TemplateProperty<const MoleculeCollection*> {
public:
    static std::string loggerCat_;

    MoleculeSelectionProperty(const std::string& id, const std::string& guiText, const MoleculeCollection* value = 0,
        int invalidationLevel=Processor::INVALID_RESULT);
    MoleculeSelectionProperty();
    virtual ~MoleculeSelectionProperty() {}

    virtual Property* create() const;

    virtual std::string getClassName() const       { return "MoleculeSelectionProperty"; }
    virtual std::string getTypeDescription() const { return "MoleculeCollection"; }
    
    /*
     * Return a collection of input molecules
     */
    const MoleculeCollection* getInputMolecules() const;
    
    /*
     * Set a collection of input molecules
     */
    void setInputMolecules(const MoleculeCollection*);
    /*
     * Return a collection of selected molecules
     */
    MoleculeCollection* getSelectedMolecules() const;
    
    /*
     * Set a collection of selected molecules
     */
    void setSelectedMolecules(MoleculeCollection*);
    
    /*
     * Checks if the given molecule is selected
     */
    bool isSelected(const Molecule*) const;
    
    /*
     * Set the selection flag for a given molecule
     */
    void setSelected(Molecule* moleculeHandle, bool selected);
    
    /*
     * Set the selection flag for all molecules
     */
    void setAllSelected(bool selected);

private:
    const MoleculeCollection* inputMolecules_; ///< Molecule collection containing all input molecules
    MoleculeCollection* selectedMolecules_; ///< Molecule collection containing selected molecules
    
};

}

#endif

