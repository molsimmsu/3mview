#include "formcharacteristicsmodule.h"

#include "processors/calculatescore.h"
#include "processors/volvolalign.h"
#include "processors/molmolalign.h"
#include "processors/formfinder.h"

namespace voreen {

FormCharacteristicsModule::FormCharacteristicsModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    setName("Form Characteristics");
    
    registerProcessor(new CalculateScore());
    registerProcessor(new VolVolAlign());
    registerProcessor(new MolMolAlign());
    registerProcessor(new FormFinder());
}

} // namespace
