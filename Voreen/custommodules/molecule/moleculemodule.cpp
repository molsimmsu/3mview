#include "moleculemodule.h"

#include "processors/moleculecollectiongeometrybuilder.h"
#include "processors/moleculecollectionsource.h"
#include "processors/moleculegeometrybuilder.h"
#include "processors/moleculeselector.h"
#include "processors/moleculesource.h"
#include "processors/moleculemanipulation.h"

namespace voreen {

MoleculeModule::MoleculeModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Molecule");
    
    registerProcessor(new MoleculeCollectionGeometryBuilder());
    registerProcessor(new MoleculeCollectionSource());
    registerProcessor(new MoleculeGeometryBuilder());
    registerProcessor(new MoleculeSelector());
    registerProcessor(new MoleculeSource());
    registerProcessor(new MoleculeManipulation());
}

} // namespace
