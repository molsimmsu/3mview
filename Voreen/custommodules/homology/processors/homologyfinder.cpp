#include "homologyfinder.h"

#include <cmath>

std::string HomologyFinder::loggerCat_ = "homology.HomologyFinder";

HomologyFinder::HomologyFinder()
    : loadSequence_("loadSequence", "Load Sequence", "Load Sequence", VoreenApplication::app()->getUserDataPath(), "*.pdb, *.fa")
    , sequenceSource_("sequenceSource", "Sequence source")
    , sequenceText_("sequenceText", "Sequence", "")
    , findDomains_("findDomains", "Find Domains")
    , alignmentList_("alignmentList", "Alignment List")
    , maxDomainsToLoad_("maxDomainsToLoad", "Max domains to load", 3, 1, 5)
    , moleculePort_(Port::INPORT, "moleculePort", "Molecule Input")
{
    addProperty(sequenceSource_);
    addProperty(loadSequence_);
    addProperty(sequenceText_);
    addProperty(findDomains_);
    addProperty(maxDomainsToLoad_);
    
    addPort(moleculePort_);
    
    sequenceSource_.addOption("string", "String");
    sequenceSource_.addOption("molecule", "Input molecule");
    // TODO sequenceSource_.addOption("fasta", "FASTA file");
    
    findDomains_.onChange(CallMemberAction<HomologyFinder>(this, &HomologyFinder::findDomains));
}

void HomologyFinder::findDomains() {
    LINFO("Start findDomains()");
    
    std::string seq = getSequence();
    if (seq.empty()) {
        LWARNING("Sequence is empty. Stop");
        return;
    }
    
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
    
    LINFO("Finish findDomains()");
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

std::string HomologyFinder::getSequence() {
    if (sequenceSource_.get() == "molecule") {
        const Molecule* mol = moleculePort_.getData();
        if (mol == 0) {
            LWARNING("Molecule is 0. Stop");
            return std::string();
        }
        
        std::vector<std::string> sequence = mol->getSequence();
        
        if (sequence.size() > 0) {
            sequenceText_.set(sequence[0]);
            return sequence[0];
        }
        else {
            LWARNING("Molecule sequence is 0.Stop");
            return std::string();
        }
    }
    
    /* TODO
    if (sequenceSource_.get() == "fasta") {
        return sequenceText_.get();
    }
    */
    
    if (sequenceSource_.get() == "string") {
        return sequenceText_.get();
    }
    
    LERROR("Wrong sequence source selected");
    return std::string();
}

