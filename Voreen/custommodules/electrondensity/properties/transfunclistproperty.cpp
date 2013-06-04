/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2012 University of Muenster, Germany.                        *
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>        *
 * For a list of authors please refer to the file "CREDITS.txt".                   *
 *                                                                                 *
 * This file is part of the Voreen software package. Voreen is free software:      *
 * you can redistribute it and/or modify it under the terms of the GNU General     *
 * Public License version 2 as published by the Free Software Foundation.          *
 *                                                                                 *
 * Voreen is distributed in the hope that it will be useful, but WITHOUT ANY       *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR   *
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.      *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License in the file   *
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                                 *
 * For non-commercial academic use see the license exception specified in the file *
 * "LICENSE-academic.txt". To get information about commercial licensing please    *
 * contact the authors.                                                            *
 *                                                                                 *
 ***********************************************************************************/

#include "transfunclistproperty.h"

#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/progressbar.h"
#include "voreen/core/voreenapplication.h"

#include "tgt/logmanager.h"
#include "tgt/filesystem.h"

namespace voreen {

const std::string TransFuncListProperty::loggerCat_("voreen.TransFuncListProperty");

TransFuncListProperty::TransFuncListProperty(const std::string& id, const std::string& guiText,
                    const std::vector<std::string>& value, bool isLoadable, int invalidationLevel) :
                    TemplateProperty<std::vector<std::string> >(id, guiText, value, invalidationLevel),
                    progressBar_(0),
                    isLoadable_(isLoadable)
{}

TransFuncListProperty::TransFuncListProperty()
    : TemplateProperty<std::vector<std::string> >("", "", std::vector<std::string>(), Processor::INVALID_RESULT)
    , progressBar_(0)
{}

Property* TransFuncListProperty::create() const {
    return new TransFuncListProperty();
}

void TransFuncListProperty::deinitialize() throw (tgt::Exception) {
    clear();
    delete progressBar_;
    progressBar_ = 0;

    TemplateProperty<std::vector<std::string> >::deinitialize();
}

void TransFuncListProperty::set(const std::vector<std::string>& URLs) {
    clear();
    TemplateProperty<std::vector<std::string> >::set(URLs);
}

void TransFuncListProperty::setURLs(const std::vector<std::string>& URLs, bool selected /*=true*/) {
    set(URLs);
    setAllSelected(selected);

    invalidate();
}

void TransFuncListProperty::addURL(const std::string& url, bool selected /*=true*/) {
    if (containsURL(url))
        return;

    value_.push_back(url);
    selectionMap_[url] = selected;

    invalidate();
}

const std::vector<std::string>& TransFuncListProperty::getURLs() const {
    return value_;
}

bool TransFuncListProperty::containsURL(const std::string& url) const {
    return std::find(value_.begin(), value_.end(), url) != value_.end();
}

void TransFuncListProperty::removeURL(const std::string& url) {
    if (!containsURL(url)) {
        LWARNING("removeURL(): passed URL not contained by this property: " << url);
        return;
    }

    // free corresponding volume, if owned by the property
    VolumeBase* handle = getVolume(url);
    if (handle)
        removeVolume(handle);

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

VolumeCollection* TransFuncListProperty::getVolumes(bool selectedOnly /*= true*/) const {
    VolumeCollection* collection = new VolumeCollection();
    std::vector<std::string> urls = get();
    for (size_t i=0; i<urls.size(); i++) {
        std::string url = urls.at(i);
        if (handleMap_.find(url) != handleMap_.end()) {
            if (!selectedOnly || (selectionMap_.find(url) != selectionMap_.end() && selectionMap_.find(url)->second == true) ) {
                VolumeBase* handle = handleMap_.find(url)->second;
                tgtAssert(handle, "handleMap_ contains null pointer");
                collection->add(handle);
            }
        }
    }

    return collection;
}

VolumeBase* TransFuncListProperty::getVolume(const std::string& url) const {
    if (!containsURL(url)) {
        LWARNING("getVolume(): passed URL not contained by this property: " << url);
        return 0;
    }

    VolumeBase* result = 0;
    if (handleMap_.find(url) != handleMap_.end()) {
        result = handleMap_.find(url)->second;
        tgtAssert(result, "handleMap_ contains null pointer");
    }

    return result;
}

void TransFuncListProperty::loadVolume(const std::string& url)
        throw (tgt::FileException, std::bad_alloc) {

    if (!containsURL(url)) {
        LWARNING("loadVolume(): passed URL not contained by this property: " << url);
        return;
    }

    // delete volume, if already loaded and owned by the property
    if (getVolume(url) && isOwner(url))
        delete getVolume(url);
    handleMap_.erase(url);
    ownerMap_.erase(url);

    ProgressBar* progressBar = getProgressBar();
    if (progressBar) {
        progressBar->setTitle("Loading volume");
        progressBar->setMessage("Loading volume ...");
    }

    VolumeSerializerPopulator serializerPopulator(progressBar);
    VolumeBase* handle = serializerPopulator.getVolumeSerializer()->read(VolumeURL(url));

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

void TransFuncListProperty::loadVolumes(bool selectedOnly /*= false*/, bool removeOnFailure /*= false*/) {

    std::vector<std::string> failedURLs;

    for (size_t i=0; i<value_.size(); i++) {
        std::string url = value_[i];
        if (selectedOnly && !isSelected(url))
            continue;
        if (getVolume(url))
            continue;

        try {
            loadVolume(url);
        }
        catch (tgt::FileException& e) {
            LERROR(e.what());
            failedURLs.push_back(url);
        }
        catch (std::bad_alloc&) {
            LERROR("bad allocation while loading volume: " << url);
            failedURLs.push_back(url);
        }
        catch (tgt::Exception& e) {
            LERROR("unknown exception while loading volume '" << url << "':" << e.what());
            failedURLs.push_back(url);
        }
    }

    if (removeOnFailure) {
        for (size_t i=0; i<failedURLs.size(); i++)
            removeURL(failedURLs.at(i));
    }
}

void TransFuncListProperty::addVolume(VolumeBase* handle, bool owner /*= false*/, bool selected /*= false*/) {
    tgtAssert(handle, "null pointer passed");

    std::string url = handle->getOrigin().getURL();
    if (!containsURL(url))
        addURL(url, selected);
    else
        removeVolume(handle);

    handleMap_[url] = handle;
    ownerMap_[url] = owner;

    invalidate();
}

void TransFuncListProperty::removeVolume(VolumeBase* handle) {
    tgtAssert(handle, "null pointer passed");
    std::string url = handle->getOrigin().getURL();
    if (!containsURL(url)) {
        LWARNING("removeVolume(): passed handle's URL not contained by this property: " << url);
        return;
    }

    if ((getVolume(url) == handle) && isOwner(url)) {
        delete handle;
    }

    handleMap_.erase(url);
    ownerMap_[url] = false;

    invalidate();
}

void TransFuncListProperty::setSelected(const std::string& url, bool selected) {
    if (!containsURL(url)) {
        LWARNING("setSelected(): passed URL not contained by this property: " << url);
        return;
    }

    if (isSelected(url) != selected) {
        selectionMap_[url] = selected;
        invalidate();
    }
}

void TransFuncListProperty::setAllSelected(bool selected) {
    for (size_t i=0; i<value_.size(); i++)
        selectionMap_[value_.at(i)] = selected;

    invalidate();
}

bool TransFuncListProperty::isSelected(const std::string& url) const {
    if (!containsURL(url)) {
        LWARNING("isURLSelected(): passed URL not contained by this property: " << url);
        return false;
    }

    return (selectionMap_.find(url) != selectionMap_.end()) && (selectionMap_.find(url)->second == true);
}

void TransFuncListProperty::serialize(XmlSerializer& s) const {
    Property::serialize(s);

    std::vector<std::string> urlList = value_;
    std::map<std::string, bool> selectMap = selectionMap_;

    // convert URLs to relative path
    std::string basePath = tgt::FileSystem::dirName(s.getDocumentPath());
    for (size_t i=0; i<urlList.size(); i++) {
        std::string url = urlList[i];
        std::string urlConv = VolumeURL::convertURLToRelativePath(url, basePath);
        urlList[i] = urlConv;
        if (selectMap.find(url) != selectMap.end()) {
            bool selected = selectMap[url];
            selectMap.erase(url);
            selectMap.insert(std::pair<std::string, bool>(urlConv, selected));
        }
    }

    s.serialize("VolumeURLs", urlList, "url");
    s.serialize("Selection", selectMap, "entry", "url");
}

void TransFuncListProperty::deserialize(XmlDeserializer& s) {
    Property::deserialize(s);

    std::vector<std::string> urlList = value_;
    std::map<std::string, bool> selectMap = selectionMap_;
    s.deserialize("VolumeURLs", urlList, "url");

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
        std::string urlConv = VolumeURL::convertURLToAbsolutePath(url, basePath);
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

void TransFuncListProperty::clear() {
    std::vector<std::string> urls = getURLs();
    for (size_t i=0; i<urls.size(); i++)
        removeURL(urls.at(i));
}

bool TransFuncListProperty::isOwner(const std::string& url) const {
    return (ownerMap_.find(url) != ownerMap_.end()) && (ownerMap_.find(url)->second == true);
}

ProgressBar* TransFuncListProperty::getProgressBar() {
    if (!progressBar_)
        progressBar_ = VoreenApplication::app()->createProgressDialog();
    return progressBar_;
}

} // namespace voreen
