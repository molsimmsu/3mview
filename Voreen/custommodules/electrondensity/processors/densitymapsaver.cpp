#include "densitymapsaver.h"
#include "voreen/core/io/volumeserializer.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumewriter.h"
#include "voreen/core/utils/stringutils.h"

namespace voreen {

const std::string DensityMapSaver::loggerCat_("3MTK.DensityMapSaver");

DensityMapSaver::DensityMapSaver()
    : folder_("outputFolder", "Folder", "Select folder", VoreenApplication::app()->getUserDataPath(),
            "Voreen Volume Data (*.vvd)", FileDialogProperty::DIRECTORY)
    , outputFormat_("outFormat","Output format")
    , saveButton_("save", "Save")
    , volSerializerPopulator_(0)
    , volumeURLList_("volumeURLList", "Volume URL List", std::vector<std::string>())
{
    folder_.onChange(CallMemberAction<DensityMapSaver>(this, &DensityMapSaver::saveVolume));
    saveButton_.onChange(CallMemberAction<DensityMapSaver>(this, &DensityMapSaver::saveVolume));
    
    outputFormat_.addOption(".vvd", "vvd");
    
    addProperty(volumeURLList_);
    addProperty(outputFormat_);
    addProperty(folder_);
    addProperty(saveButton_);
}

void DensityMapSaver::updateSelection() {
    DensityMapCoProcessor::updateSelection();
    const VolumeCollection* collection = getInputVolumeCollection();
    if (collection == 0) {
        LERROR("Collection is NULL at DensityMapManipulation::updateSelection()");
        return;
    }
    volumeURLList_.clear();
    for (size_t i = 0; i < collection->size(); i++)
        volumeURLList_.addVolume(collection->at(i));
}

DensityMapSaver::~DensityMapSaver() {
}

Processor* DensityMapSaver::create() const {
    return new DensityMapSaver();
}

void DensityMapSaver::initialize() throw (tgt::Exception) {
    DensityMapCoProcessor::initialize();

    tgtAssert(!volSerializerPopulator_, "VolumeSerializerPopulator already created");
    volSerializerPopulator_ = new VolumeSerializerPopulator();
    std::vector<std::string> filters = constructFilters();
    folder_.setFileFilter(strJoin(filters, ";;"));
}

void DensityMapSaver::deinitialize() throw (tgt::Exception) {
    delete volSerializerPopulator_;
    volSerializerPopulator_ = 0;

    DensityMapCoProcessor::deinitialize();
}

void DensityMapSaver::process() {

}

void DensityMapSaver::saveVolume() {

    if (folder_.get() == "") {
        LWARNING("no filename specified");
        return;
    }
    
    LINFO(folder_.get());
    
    VolumeCollection* volumes = volumeURLList_.getVolumes(true);
    try {
        tgtAssert(volSerializerPopulator_, "VolumeSerializerPopulator not instantiated");
        tgtAssert(volSerializerPopulator_->getVolumeSerializer(), "no VolumeSerializer");
        
        for (size_t i = 0; i < volumes->size(); i++) {
            VolumeBase* vol = volumes->at(i);
            std::string fileName = vol->getOrigin().getFilename();
            size_t dotPos = fileName.find_last_of('.');
            fileName = fileName.substr(0, dotPos) + outputFormat_.get();
            volSerializerPopulator_->getVolumeSerializer()->write(folder_.get() + "/" + fileName, vol);
        }
    }
    catch(tgt::FileException e) {
        LERROR(e.what());
        folder_.set("");
    }
}

std::vector<std::string> DensityMapSaver::constructFilters() const {
    tgtAssert(volSerializerPopulator_, "VolumeSerializerPopualator not instantiated");
    std::vector<std::string> filters;

    const std::vector<VolumeWriter*> volumeWriters = volSerializerPopulator_->getVolumeSerializer()->getWriters();
    std::string vvdWriterFilter;
    for (size_t i=0; i<volumeWriters.size(); i++) {
        VolumeWriter* curWriter = volumeWriters.at(i);

        // extensions
        std::vector<std::string> extensionVec = curWriter->getSupportedExtensions();
        for (size_t j=0; j<extensionVec.size(); j++) {
            std::string extension = extensionVec.at(j);
            std::string filterStr = curWriter->getFormatDescription() + " (*." + extension + ")";
            if (extension == "vvd")
                vvdWriterFilter = filterStr;
            else
                filters.push_back(filterStr);
        }

        // filename
        std::vector<std::string> filenamesVec = curWriter->getSupportedFilenames();
        for (size_t j=0; j<filenamesVec.size(); j++) {
            std::string filename = filenamesVec.at(j);
            std::string filterStr = curWriter->getFormatDescription() + " (" + filename + ")";
            filters.push_back(filterStr);
        }

    }

    if (vvdWriterFilter != "")
        filters.insert(filters.begin(), vvdWriterFilter);

    return filters;
}

}   // namespace
