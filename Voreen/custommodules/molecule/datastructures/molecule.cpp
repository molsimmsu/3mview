#include "molecule.h"

namespace voreen {
  
const std::string Molecule::loggerCat_ = "voreen.Molecule";

Molecule::Molecule() {
    transformationMatrix_ = tgt::mat4::createIdentity();
}

Molecule::Molecule(const OBMol& mol)
  : mol_(mol)
{
    transformationMatrix_ = tgt::mat4::createIdentity();
}

Molecule::Molecule(const OBMol& mol, const SecStructure& secStructure)
  : mol_(mol)
  , secStructure_(secStructure)
{}

Molecule* Molecule::clone() const {
    return new Molecule(mol_);
}

const OBMol& Molecule::getOBMol() const {
    return mol_;
}

void Molecule::ClearResidues(int restype, bool invert){
    
        std::cout << mol_.NumAtoms() << std::endl;
        std::cout << mol_.NumResidues() << std::endl;
        /*for(int i=mol_.NumAtoms(); i>0; i--){
            OBAtom * atom = mol_.GetAtom(i);
            if(!(invert == atom->GetResidue()->GetResidueProperty(restype))){ mol_.DeleteAtom(atom);
            } 
        }*/
        for(int i=mol_.NumResidues()-1; i>=0; i--){
            OBResidue * res = mol_.GetResidue(i);
            if(!(invert == res->GetResidueProperty(restype))){ 
                std::vector<OBAtom *> atoms = res->GetAtoms();
                for(int a=0; a<atoms.size(); a++){
                    mol_.DeleteAtom(atoms[a]);
                }
                mol_.DeleteResidue(res);
            } 
        }
        std::cout << mol_.NumAtoms() << std::endl;
        std::cout << mol_.NumResidues() << std::endl;
        
}

char Molecule::getSecondaryStructure(size_t chainNum, size_t residueNum) const {
	return secStructure_[chainNum][residueNum];
}

const tgt::mat4& Molecule::getTransformationMatrix() const {
    return transformationMatrix_;
}
     
void Molecule::transform(const tgt::mat4& matrix) {
    transformationMatrix_ = matrix * transformationMatrix_;
    notifyTransformationChange(matrix);
}

const VolumeURL& Molecule::getOrigin() const {
    return origin_;
}

VolumeURL& Molecule::getOrigin() {
    return origin_;
}

void Molecule::setOrigin(const VolumeURL& origin) {
    origin_ = origin;
}

void Molecule::notifyDelete() {
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeDelete(this);
}

void Molecule::notifyReload() {
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i)
        observers[i]->moleculeChange(this);
}

void Molecule::notifyTransformationChange(const tgt::mat4& matrix) {
    std::vector<MoleculeObserver*> observers = getObservers();
    for (size_t i=0; i<observers.size(); ++i) {
        observers[i]->moleculeTransform(this, matrix);
    }
}

} // namespace voreen
