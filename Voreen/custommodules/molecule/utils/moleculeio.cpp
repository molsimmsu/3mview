#include "moleculeio.h"

#include "../utils/stridereader.h"

#include "openbabel/obconversion.h"
#include "openbabel/mol.h"
#include "openbabel/residue.h"
#include "openbabel/obiter.h"

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


void MoleculeIO::write(Molecule * molecule, const std::string& filename)
{
    tgt::mat4 matrix = molecule->getTransformationMatrix(); 
    OBMol mol = molecule->clone()->getOBMol();
    //perform rotation of all atoms
    
     FOR_ATOMS_OF_MOL(a, mol)
     {
        vector3 pos = a->GetVector();
        tgt::vec3 pos2 = tgt::vec3(pos[0],pos[1],pos[2]);
        pos2 = matrix * pos2;
        pos= vector3(pos2[0],pos2[1],pos2[2]);
        a->SetVector(pos);
     }
    
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

