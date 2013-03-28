#ifndef MOLECULEIO_H
#define MOLECULEIO_H

#include "../datastructures/molecule.h"
using namespace voreen;

#include "tgt/exception.h"
using tgt::FileException;

class MoleculeIO
{
public:
    MoleculeIO();
    
    static Molecule* read(const MoleculeURL& url);
    static Molecule* read(const std::string& filename)
        throw (FileException);
        
    static void write(OBMol& mol, const std::string& filename);
    static std::string getAminoFasta(std::string name);
    static std::vector<std::string> getFastaFromMol(OBMol mol);
protected:
    static const std::string loggerCat_;

};

#endif // MOLECULEREADER_H
