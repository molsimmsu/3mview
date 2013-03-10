#include "domaindatabaseadministration.h"

std::string DomainDatabaseAdministration::loggerCat_ = "homology.DomainDatabaseAdministration";

DomainDatabaseAdministration::DomainDatabaseAdministration()
    : selectDomain_("selectDomain", "Select Domain", "Select a PDB file", VoreenApplication::app()->getUserDataPath(), "*.pdb")
    , addDomain_("addDomain", "Add Domain")
{
    addProperty(selectDomain_);
    addProperty(addDomain_);
    
    addDomain_.onChange(CallMemberAction<DomainDatabaseAdministration>(this, &DomainDatabaseAdministration::addDomain));
}

void DomainDatabaseAdministration::addDomain() {
    // Get source domain path
    std::string srcPath = selectDomain_.get();
    // Get file name
    std::string srcFileName = tgt::FileSystem::fileName(srcPath);
    // Set destination file name
    std::string destFileName = srcFileName;
    // Get target directory
    std::string destPath = VoreenApplication::app()->getUserDataPath() + "/DomainDB/domains/" + destFileName;
    // Copy source to domains folder
    tgt::FileSystem::copyFile(srcPath, destPath);
    // Put sequence into the domains.fa file
}
