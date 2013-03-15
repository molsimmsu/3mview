
#include "formcharacteristicsmodule.h"
#include "processors/alignbymoments.h"
#include "processors/databasematch.h"
#include "processors/calculatescore.h"

namespace voreen {

FormCharacteristicsModule::FormCharacteristicsModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    // module name to be used in the GUI
    setName("Form Characteristics");
    
    registerProcessor(new AlignByMoments());
    registerProcessor(new DatabaseMatch());
    registerProcessor(new CalculateScore());
}

} // namespace
