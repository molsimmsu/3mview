#include "homologyfinder.h"

#include <cmath>

std::string HomologyFinder::loggerCat_ = "homology.HomologyFinder";

HomologyFinder::HomologyFinder()
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
    
    findDomains_.onChange(CallMemberAction<HomologyFinder>(this, &HomologyFinder::findDomains));
}

void HomologyFinder::findDomains() {
    std::string seq = sequenceText_.get();
    LINFO("Finding domains with sequence:");
    LINFO(seq.c_str());
    
	std::string cmd("cd ../../DomainDB/bin && ./bdf -f ");
	cmd += seq;
	
	AlignmentList A;
	
	FILE* pipe = popen(cmd.c_str(), "r");

	while (!feof(pipe))
	{
		char sseqid[8]; float pident;
		int qstart, qend;
		fscanf(pipe, "%s %f %d %d", sseqid, &pident, &qstart, &qend);
		A.push_back(Alignment(sseqid, pident, qstart, qend));
	}
	
	A.pop_back();
	
	pclose(pipe);
	
	alignmentList_.set(A);
    
    loadDomains();
}

void HomologyFinder::loadDomains() {
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
        std::stringstream pdbPath;
        pdbPath << path << a.getName() << ".pdb" << "?score=" << a.pident;
        LINFO(pdbPath.str());
        molCollection->load(pdbPath.str());
    }
}
