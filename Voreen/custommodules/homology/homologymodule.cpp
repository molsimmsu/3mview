#include "homologymodule.h"

#include "processors/domaindatabaseadministration.h"
#include "processors/multiplesequencefinder.h"

namespace voreen {

HomologyModule::HomologyModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Homology");
    
    registerProcessor(new DomainDatabaseAdministration());
    registerProcessor(new MultipleSequenceFinder());
}

} // namespace
