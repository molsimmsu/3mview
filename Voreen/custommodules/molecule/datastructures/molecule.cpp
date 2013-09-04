#include "molecule.h"

namespace voreen {
  
const std::string Molecule::loggerCat_ = "voreen.Molecule";

Molecule::Molecule()
  : rep_(new RibbonsRep()) 
{
    transformationMatrix_ = tgt::mat4::createIdentity();
    this->CalcCenterOfMass();
}

Molecule::Molecule(const OBMol& mol)
  : mol_(mol)
  , rep_(new RibbonsRep())
{
    transformationMatrix_ = tgt::mat4::createIdentity();
    this->CalcCenterOfMass();
}

Molecule::Molecule(const OBMol& mol, const SecStructure& secStructure)
  : mol_(mol)
  , rep_(new RibbonsRep())
  , secStructure_(secStructure)
{}

Molecule* Molecule::clone() const {
    return new Molecule(mol_);
}

const OBMol& Molecule::getOBMol() const {
    return mol_;
}

size_t Molecule::numAtoms() const {
    return mol_.NumAtoms();
}
    
Atom* Molecule::atom(size_t i) const {
    OBAtom* a = mol_.GetAtom(i + 1);
    return createAtomFromOBAtom(a);
}

Atom* Molecule::createAtomFromOBAtom(OBAtom* a) const {
    std::string srcType, dstType;
    srcType = a->GetType();
    
    ttab.SetFromType("INT");
    ttab.SetToType("XYZ");
    ttab.Translate(dstType,srcType);
    
    return new Atom(std::string(dstType), a->x(), a->y(), a->z(), a->GetAtomicNum());
}

size_t Molecule::numBonds() const {
    return mol_.NumBonds();
}
    
Bond* Molecule::bond(size_t i) const {
    OBBond* bond = mol_.GetBond(i);
    OBAtom* a1 = bond->GetBeginAtom();
    OBAtom* a2 = bond->GetEndAtom();
    return new Bond(atom(a1->GetIdx()-1), atom(a2->GetIdx()-1));
}

size_t Molecule::numResidues() const {
    return mol_.NumResidues();
}

MoleculeResidue* Molecule::residue(size_t i) const {
    OBResidue* obres = mol_.GetResidue(i);
    std::vector<OBAtom*> atoms = obres->GetAtoms();
    MoleculeResidue* res = new MoleculeResidue(obres->GetChainNum());
    
    for (size_t i = 0; i < atoms.size(); i++)
        res->addAtom(createAtomFromOBAtom(atoms[i]), obres->GetAtomID(atoms[i]));
    
    return res;
}

void Molecule::DeleteHydrogens() {
mol_.DeleteHydrogens();
notifyReload();
}

void Molecule::CalcCenterOfMass() {
    float totalmass = 0;
    MassCent_ = tgt::vec3(0.f, 0.f, 0.f);
    FOR_ATOMS_OF_MOL(a,mol_){
        float mass = a->GetAtomicMass();
        MassCent_ = MassCent_ + tgt::vec3(mass*a->x(), mass*a->y(),mass*a->z());
        totalmass += mass;
    }
    MassCent_ /= totalmass;

}

const tgt::vec3 Molecule::getCenterOfMass(){
    return MassCent_;
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
        
        notifyReload();
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
    MassCent_ = matrix * MassCent_;
    notifyTransformationChange(matrix);
}

void Molecule::addMolecule(Molecule * molecule){
    Molecule *mol = molecule->clone();
    mol->setTransformationMatrix(molecule->getTransformationMatrix());
    mol->updateCoordinates();
    OBMol inmol = mol->getOBMol();
    mol_ += inmol;
}

void Molecule::updateCoordinates(){
     FOR_ATOMS_OF_MOL(a, mol_)
     {
        vector3 pos = a->GetVector();
        tgt::vec3 pos2 = tgt::vec3(pos[0],pos[1],pos[2]);
        pos2 = transformationMatrix_ * pos2;
        pos= vector3(pos2[0],pos2[1],pos2[2]);
        a->SetVector(pos);
     }
     setTransformationMatrix(tgt::mat4::createIdentity());
     notifyReload();
}

void Molecule::setTransformationMatrix(const tgt::mat4& transformationMatrix) {
    tgt::mat4 oldMatrix = getTransformationMatrix();
    tgt::mat4 invertMatrix;
    oldMatrix.invert(invertMatrix);
    tgt::mat4 molTransform = transformationMatrix * invertMatrix;
    
    transform(molTransform);
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
