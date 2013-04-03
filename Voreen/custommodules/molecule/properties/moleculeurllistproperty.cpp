#include "moleculeurllistproperty.h"

#include "../datastructures/moleculecollection.h"
#include "../utils/moleculeio.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"

#include "tgt/logmanager.h"
#include "tgt/filesystem.h"

namespace voreen {

const std::string MoleculeURLListProperty::loggerCat_("voreen.MoleculeURLListProperty");

MoleculeURLListProperty::MoleculeURLListProperty(const std::string& id, const std::string& guiText,
                    const std::vector<std::string>& value, bool isLoadable, int invalidationLevel) :
                    TemplateProperty<std::vector<std::string> >(id, guiText, value, invalidationLevel),
                    progressBar_(0),
                    isLoadable_(isLoadable)
{}

MoleculeURLListProperty::MoleculeURLListProperty()
    : TemplateProperty<std::vector<std::string> >("", "", std::vector<std::string>(), Processor::INVALID_RESULT)
    , progressBar_(0)
{}

Property* MoleculeURLListProperty::create() const {
    return new MoleculeURLListProperty();
}

void MoleculeURLListProperty::deinitialize() throw (tgt::Exception) {
    clear();
    delete progressBar_;
    progressBar_ = 0;

    TemplateProperty<std::vector<std::string> >::deinitialize();
}

void MoleculeURLListProperty::set(const std::vector<std::string>& URLs) {
    clear();
    TemplateProperty<std::vector<std::string> >::set(URLs);
}

void MoleculeURLListProperty::setURLs(const std::vector<std::string>& URLs, bool selected /*=true*/) {
    set(URLs);
    setAllSelected(selected);

    invalidate();
}

void MoleculeURLListProperty::addURL(const std::string& url, bool selected /*=true*/) {
    if (containsURL(url))
        return;

    value_.push_back(url);
    selectionMap_[url] = selected;

    invalidate();
}

const std::vector<std::string>& MoleculeURLListProperty::getURLs() const {
    return value_;
}

bool MoleculeURLListProperty::containsURL(const std::string& url) const {
    return std::find(value_.begin(), value_.end(), url) != value_.end();
}

void MoleculeURLListProperty::removeURL(const std::string& url) {
    if (!containsURL(url)) {
        LWARNING("removeURL(): passed URL not contained by this property: " << url);
        return;
    }

    // free corresponding molecule, if owned by the property
    Molecule* handle = getMolecule(url);
    if (handle)
        removeMolecule(handle);

    // remove url from url list
    std::vector<std::string>::iterator it = std::find(value_.begin(), value_.end(), url);
    tgtAssert(it != value_.end(), "url not found");
    value_.erase(it);

    // remove url from maps
    handleMap_.erase(url);
    selectionMap_.erase(url);
    ownerMap_.erase(url);

    invalidate();
}

MoleculeCollection* MoleculeURLListProperty::getMolecules(bool selectedOnly /*= true*/) const {
    MoleculeCollection* collection = new MoleculeCollection();
    std::vector<std::string> urls = get();
    for (size_t i=0; i<urls.size(); i++) {
        std::string url = urls.at(i);
        if (handleMap_.find(url) != handleMap_.end()) {
            if (!selectedOnly || (selectionMap_.find(url) != selectionMap_.end() && selectionMap_.find(url)->second == true) ) {
                Molecule* handle = handleMap_.find(url)->second;
                tgtAssert(handle, "handleMap_ contains null pointer");
                collection->add(handle);
            }
        }
    }

    return collection;
}

Molecule* MoleculeURLListProperty::getMolecule(const std::string& url) const {
    if (!containsURL(url)) {
        LWARNING("getMolecule(): passed URL not contained by this property: " << url);
        return 0;
    }

    Molecule* result = 0;
    if (handleMap_.find(url) != handleMap_.end()) {
        result = handleMap_.find(url)->second;
        tgtAssert(result, "handleMap_ contains null pointer");
    }

    return result;
}

void MoleculeURLListProperty::loadMolecule(const std::string& url)
        throw (tgt::FileException, std::bad_alloc) {

    if (!containsURL(url)) {
        LWARNING("loadMolecule(): passed URL not contained by this property: " << url);
        return;
    }

    // delete molecule, if already loaded and owned by the property
    if (getMolecule(url) && isOwner(url))
        delete getMolecule(url);
    handleMap_.erase(url);
    ownerMap_.erase(url);

    ProgressBar* progressBar = getProgressBar();
    if (progressBar) {
        progressBar->setTitle("Loading molecule");
        progressBar->setMessage("Loading molecule ...");
    }

    Molecule* handle = MoleculeIO::read(MoleculeURL(url));

    if (handle) {
        // url may have been altered by loading routine
        if (url != handle->getOrigin().getURL()) {
            bool selected = isSelected(url);
            selectionMap_.erase(url);
            selectionMap_[handle->getOrigin().getURL()] = selected;

            for (size_t i=0; i<value_.size(); i++) {
                if (value_[i] == url) {
                    value_[i] = handle->getOrigin().getURL();
                    break;

                }
            }
        }

        handleMap_[handle->getOrigin().getURL()] = handle;
        ownerMap_[handle->getOrigin().getURL()] = true;
    }

    invalidate();
}

void MoleculeURLListProperty::loadMolecules(bool selectedOnly /*= false*/, bool removeOnFailure /*= false*/) {

    std::vector<std::string> failedURLs;

    for (size_t i=0; i<value_.size(); i++) {
        std::string url = value_[i];
        if (selectedOnly && !isSelected(url))
            continue;
        if (getMolecule(url))
            continue;

        try {
            loadMolecule(url);
        }
        catch (tgt::FileException& e) {
            LERROR(e.what());
            failedURLs.push_back(url);
        }
        catch (std::bad_alloc&) {
            LERROR("bad allocation while loading molecule: " << url);
            failedURLs.push_back(url);
        }
        catch (tgt::Exception& e) {
            LERROR("unknown exception while loading molecule '" << url << "':" << e.what());
            failedURLs.push_back(url);
        }
    }

    if (removeOnFailure) {
        for (size_t i=0; i<failedURLs.size(); i++)
            removeURL(failedURLs.at(i));
    }
}

void MoleculeURLListProperty::addMolecule(Molecule* handle, bool owner /*= false*/, bool selected /*= false*/) {
    tgtAssert(handle, "null pointer passed");

    std::string url = handle->getOrigin().getURL();
    if (!containsURL(url))
        addURL(url, selected);
    else
        removeMolecule(handle);

    handleMap_[url] = handle;
    ownerMap_[url] = owner;

    invalidate();
}

void MoleculeURLListProperty::removeMolecule(Molecule* handle) {
    tgtAssert(handle, "null pointer passed");
    std::string url = handle->getOrigin().getURL();
    if (!containsURL(url)) {
        LWARNING("removeMolecule(): passed handle's URL not contained by this property: " << url);
        return;
    }

    if ((getMolecule(url) == handle) && isOwner(url)) {
        delete handle;
    }

    handleMap_.erase(url);
    ownerMap_[url] = false;

    invalidate();
}

void MoleculeURLListProperty::setSelected(const std::string& url, bool selected) {
    if (!containsURL(url)) {
        LWARNING("setSelected(): passed URL not contained by this property: " << url);
        return;
    }

    if (isSelected(url) != selected) {
        selectionMap_[url] = selected;
        invalidate();
    }
}

void MoleculeURLListProperty::setAllSelected(bool selected) {
    for (size_t i=0; i<value_.size(); i++)
        selectionMap_[value_.at(i)] = selected;

    invalidate();
}

bool MoleculeURLListProperty::isSelected(const std::string& url) const {
    if (!containsURL(url)) {
        LWARNING("isURLSelected(): passed URL not contained by this property: " << url);
        return false;
    }

    return (selectionMap_.find(url) != selectionMap_.end()) && (selectionMap_.find(url)->second == true);
}

void MoleculeURLListProperty::serialize(XmlSerializer& s) const {
    Property::serialize(s);

    std::vector<std::string> urlList = value_;
    std::map<std::string, bool> selectMap = selectionMap_;

    // convert URLs to relative path
    std::string basePath = tgt::FileSystem::dirName(s.getDocumentPath());
    for (size_t i=0; i<urlList.size(); i++) {
        std::string url = urlList[i];
        std::string urlConv = MoleculeURL::convertURLToRelativePath(url, basePath);
        urlList[i] = urlConv;
        if (selectMap.find(url) != selectMap.end()) {
            bool selected = selectMap[url];
            selectMap.erase(url);
            selectMap.insert(std::pair<std::string, bool>(urlConv, selected));
        }
    }

    s.serialize("MoleculeURLs", urlList, "url");
    s.serialize("Selection", selectMap, "entry", "url");
}

void MoleculeURLListProperty::deserialize(XmlDeserializer& s) {
    Property::deserialize(s);

    std::vector<std::string> urlList = value_;
    std::map<std::string, bool> selectMap = selectionMap_;
    s.deserialize("MoleculeURLs", urlList, "url");

    try {
        s.deserialize("Selection", selectMap, "entry", "url");
    }
    catch (SerializationException& e) {
        s.removeLastError();
        LWARNING("Failed to deserialize selection map: " << e.what());
    }

    // convert URLs to absolute path
    std::string basePath = tgt::FileSystem::dirName(s.getDocumentPath());
    for (size_t i=0; i<urlList.size(); i++) {
        std::string url = urlList[i];
        std::string urlConv = MoleculeURL::convertURLToAbsolutePath(url, basePath);
        urlList[i] = urlConv;
        if (selectMap.find(url) != selectMap.end()) {
            bool selected = selectMap[url];
            selectMap.erase(url);
            selectMap.insert(std::pair<std::string, bool>(urlConv, selected));
        }
    }

    value_ = urlList;
    selectionMap_ = selectMap;

    invalidate();
}

void MoleculeURLListProperty::clear() {
    std::vector<std::string> urls = getURLs();
    for (size_t i=0; i<urls.size(); i++)
        removeURL(urls.at(i));
}

bool MoleculeURLListProperty::isOwner(const std::string& url) const {
    return (ownerMap_.find(url) != ownerMap_.end()) && (ownerMap_.find(url)->second == true);
}

ProgressBar* MoleculeURLListProperty::getProgressBar() {
    if (!progressBar_)
        progressBar_ = VoreenApplication::app()->createProgressDialog();
    return progressBar_;
}

} // namespace voreen
