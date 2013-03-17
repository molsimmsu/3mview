#ifndef MOLECULEREADER_H
#define MOLECULEREADER_H

#include "../datastructures/molecule.h"
using namespace voreen;

#include "tgt/exception.h"
using tgt::FileException;

class MoleculeReader
{
public:
    MoleculeReader();
    
    static Molecule* read(const MoleculeURL& url);
    static Molecule* read(const std::string& filename)
        throw (FileException);
  
protected:
    static const std::string loggerCat_;

};

#endif // MOLECULEREADER_H
