#include "formcharacteristicsmodule.h"
#include "processors/alignbymoments.h"
#include "processors/databasematch.h"
#include "processors/calculatescore_.h"
#include "processors/volvolalign.h"
#include "processors/molmolalign.h"
#include "processors/formfinder.h"

namespace voreen {

FormCharacteristicsModule::FormCharacteristicsModule(const std::string& modulePath) 
    : VoreenModule(modulePath)
{
    // module name to be used in the GUI
    setName("Form Characteristics");
    
    registerProcessor(new AlignByMoments());
    registerProcessor(new DatabaseMatch());
    registerProcessor(new CalculateScore());
    registerProcessor(new VolVolAlign());
    registerProcessor(new MolMolAlign());
    registerProcessor(new FormFinder());
}

} // namespace
