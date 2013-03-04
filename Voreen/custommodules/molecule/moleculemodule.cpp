#include "moleculemodule.h"

// include classes to be registered
#include "processors/moleculecollectiongeometrybuilder.h"
#include "processors/moleculecollectionsource.h"
#include "processors/moleculegeometrybuilder.h"
#include "processors/moleculeselector.h"
#include "processors/moleculesource.h"
#include "processors/moleculetransformer.h"

namespace voreen {

MoleculeModule::MoleculeModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    // module name to be used in the GUI
    setName("Molecule");
    
    // each module processor needs to be registered
    registerProcessor(new MoleculeCollectionGeometryBuilder());
    registerProcessor(new MoleculeCollectionSource());
    registerProcessor(new MoleculeGeometryBuilder());
    registerProcessor(new MoleculeSelector());
    registerProcessor(new MoleculeSource());
}

} // namespace
