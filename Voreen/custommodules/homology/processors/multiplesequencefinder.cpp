#include "multiplesequencefinder.h"
#include "Alignment.h"

std::string MultipleSequenceFinder::loggerCat_ = "homology.MultipleSequenceFinder";

MultipleSequenceFinder::MultipleSequenceFinder()
    : loadSequence_("loadSequence", "Load Sequence", "Load Sequence", VoreenApplication::app()->getUserDataPath(), "*.pdb, *.fa")
    , sequenceText_("sequenceText", "Sequence", "")
    , findDomains_("findDomains", "Find Domains")
{
    addProperty(loadSequence_);
    addProperty(sequenceText_);
    addProperty(findDomains_);
    
    findDomains_.onChange(CallMemberAction<MultipleSequenceFinder>(this, &MultipleSequenceFinder::findDomains));
}

void MultipleSequenceFinder::findDomains() {
    std::string seq = sequenceText_.get();
    LINFO("Finding domains with sequence:");
    LINFO(seq.c_str());
    
	AlignmentList A;
	A.findHomologousBLAST(seq, "../../DomainDB/bin");
	A.sortByIdentityPercent();
	A.write(std::cout);
}
