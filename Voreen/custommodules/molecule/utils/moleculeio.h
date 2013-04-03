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
    static Molecule* read(const std::string& filename);

    static void write(Molecule * molecule, const std::string& filename, bool cleanGenericData = false);
    
    
protected:
    static const std::string loggerCat_;

};

#endif // MOLECULEREADER_H
