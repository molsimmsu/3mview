#include "multiplesequencefinder.h"

#include <cmath>

std::string MultipleSequenceFinder::loggerCat_ = "homology.MultipleSequenceFinder";

MultipleSequenceFinder::MultipleSequenceFinder()
    : loadSequence_("loadSequence", "Load Sequence", "Load Sequence", VoreenApplication::app()->getUserDataPath(), "*.pdb, *.fa")
    , sequenceText_("sequenceText", "Sequence", "")
    , findDomains_("findDomains", "Find Domains")
    , alignmentList_("alignmentList", "Alignment List")
    , maxDomainsToLoad_("maxDomainsToLoad", "Max domains to load", 3, 1, 5)
{
    addProperty(loadSequence_);
    addProperty(sequenceText_);
    addProperty(findDomains_);
    addProperty(maxDomainsToLoad_);
    
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
	
    alignmentList_.set(A);
    
    loadDomains();
}

void MultipleSequenceFinder::loadDomains() {
    MoleculeCollectionSource* molCollection = getSourceProcessor();
    if (molCollection == 0) return;

    const AlignmentList& A = alignmentList_.get();
    std::cout << A.size() << std::endl;
    unsigned int maxToLoad = maxDomainsToLoad_.get();
    int numToLoad = min(maxToLoad, A.size());
    
    std::string path("../../DomainDB/bin/DomainsDB/domains/");
    
    LINFO("Loading domains:");
    for (size_t i = 0; i < numToLoad; i++) {
        Alignment a = A.at(i);
        std::string pdbPath = path + a.getName() + ".pdb";
        LINFO(pdbPath.c_str());
        molCollection->load(pdbPath);
    }
}
