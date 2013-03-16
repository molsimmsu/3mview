#include "homologymodule.h"

#include "processors/domaindatabaseadministration.h"
#include "processors/homologyfinder.h"

namespace voreen {

HomologyModule::HomologyModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Homology");
    
    registerProcessor(new DomainDatabaseAdministration());
    registerProcessor(new HomologyFinder());
}

} // namespace
