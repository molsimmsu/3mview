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

void Molecule::DeleteHydrogens() {
mol_.DeleteHydrogens();
}

void Molecule::clearResidues(int restype, bool invert) {
    
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

std::vector<std::string> Molecule::getSequence() const {
    const OBMol& mol = getOBMol();

    std::vector<std::string> fasta;
    //std::cout << "getFastaFromMol()" << std::endl;
    
    size_t numResidues = mol.NumResidues();
    //std::cout << numResidues << " residues" << std::endl;
    
    for (size_t i = 0; i < numResidues; i++) {
        OBResidue* res = mol.GetResidue(i);
        //std::cout << "Residue" << i << std::endl;
        
        if (res->GetResidueProperty(0)) { //checks that this residue belongs to protein 
            std::string resname = res->GetName();
            
            //std::cout << "Residue is protein" << std::endl;
            
            int chainNum = res->GetChainNum();
            //std::cout << "Chain num is " << chainNum << std::endl;
            
            while (chainNum > fasta.size())
                fasta.push_back(std::string());
            
            std::string aminoFasta = getAminoFasta(resname);
            //std::cout << "Amino Fasta is " << aminoFasta << std::endl;
            
            fasta[chainNum-1].append(aminoFasta);
            //std::cout << "Append OK" << std::endl;
        }
    }
    
    // Return only non-empty chains
    std::vector<std::string> result;
    for (size_t i = 0; i < fasta.size(); i++)
        if (fasta[i].size() > 0)
            result.push_back(fasta[i]);
    
    return result;
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

std::string Molecule::getAminoFasta(std::string name) {
    std::string resfasta;
    if ( name.compare("ALA")==0) resfasta = "A";
        else if ( name.compare("ARG")==0) resfasta = "R";
        else if ( name.compare("ASN")==0) resfasta = "N";
        else if ( name.compare("ASP")==0) resfasta = "D";
        else if ( name.compare("ASX")==0) resfasta = "B";
        else if ( name.compare("CYS")==0) resfasta = "C";
        else if ( name.compare("GLN")==0) resfasta = "Q";
        else if ( name.compare("GLU")==0) resfasta = "E";
        else if ( name.compare("GLX")==0) resfasta = "Z";
        else if ( name.compare("GLY")==0) resfasta = "G";
        else if ( name.compare("HIS")==0) resfasta = "H";
        else if ( name.compare("ILE")==0) resfasta = "I";
        else if ( name.compare("LEU")==0) resfasta = "L";
        else if ( name.compare("LYS")==0) resfasta = "K";
        else if ( name.compare("MET")==0) resfasta = "M";
        else if ( name.compare("PHE")==0) resfasta = "F";
        else if ( name.compare("PRO")==0) resfasta = "P";
        else if ( name.compare("SER")==0) resfasta = "S";
        else if ( name.compare("THR")==0) resfasta = "T";
        else if ( name.compare("TRP")==0) resfasta = "W";
        else if ( name.compare("TYR")==0) resfasta = "Y";
        else if ( name.compare("VAL")==0) resfasta = "V";
        else if ( name.compare("PCA")==0) resfasta = "X";
        else resfasta = "-";
        return resfasta;
}

} // namespace voreen
