#include "moleculeio.h"

#include "../utils/stridereader.h"

#include "openbabel/obconversion.h"
#include "openbabel/mol.h"
#include "openbabel/residue.h"

using namespace OpenBabel;

#include <fstream>
#include <string>

const std::string MoleculeIO::loggerCat_ = "3mview.MoleculeIO";

Molecule* MoleculeIO::read(const std::string& filename) {
    return read(VolumeURL(filename));
}

Molecule* MoleculeIO::read(const MoleculeURL& url) {
    std::string filename = url.getPath();

    std::ifstream stream;
    stream.open(filename.c_str());
    if (stream.fail())
        throw FileException("Failed to open file for reading ", filename);

    std::string molFormat("PDB");

    LWARNING("Start loading");

    OBConversion conv;
    if (!conv.SetInFormat(molFormat.c_str()))
        throw FileException("Failed to set input format for reading molecule", filename);
        
    LWARNING("Start conversion");
        
    OBMol mol;
    if (!conv.Read(&mol, &stream))
        throw FileException("Failed to read molecule from file", filename);
        
    //LWARNING("Start STRIDE");
    
    // FIXME StrideReader stride(filename.c_str());
    
    LWARNING("Loading successful");
    
    //return new Molecule(mol, stride.getStructure());
    Molecule* result = new Molecule(mol);
    result->setOrigin(url);
    return result;
}

void MoleculeIO::write(OBMol& mol, const std::string& filename)
{   
    std::fstream stream;
    stream.open(filename.c_str(), std::ios_base::out);

    if (stream.fail())
        throw VoreenException("Failed to open file for witing: " + filename);

    std::string molFormat("PDB");

    LWARNING("Start writing");

    OBConversion conv;
    if (!conv.SetOutFormat(molFormat.c_str()))
        throw VoreenException("Failed to set output format for writing molecule: " + molFormat);
        
    LWARNING("Start conversion");
        
    
    if (!conv.Write(&mol, &stream)==1)
        throw VoreenException("Failed to write molecule from file: " + filename);

    stream.close();
        
    
    LWARNING("Write successful");
 
}

std::string MoleculeIO::getAminoFasta(std::string name){
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


// returns string with FASTA formated sequence 
std::vector<std::string> MoleculeIO::getFastaFromMol(OBMol mol){

    OBResidueIterator  i;
    std::vector<std::string> fasta;

    fasta.push_back(getAminoFasta(mol.BeginResidue(i)->GetName()));

    while(true){
        OBResidue *res = mol.NextResidue(i);
        if(res==0) break;
        if (res->GetResidueProperty(0)){ //checks that this residue belongs to protein 
            std::string resname = res->GetName();
            
            if (res->GetChainNum() > fasta.size())
                fasta.push_back(std::string());
                
            fasta[res->GetChainNum()-1].append(getAminoFasta(resname));
        }
    }
    return fasta;
}











