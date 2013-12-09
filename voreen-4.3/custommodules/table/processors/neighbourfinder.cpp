#include "neighbourfinder.h"

namespace voreen {

const std::string NeighbourFinder::loggerCat_("voreen.base.NeighbourFinder");

NeighbourFinder::NeighbourFinder()
    : Processor(),
    inport_(Port::INPORT, "volumehandle.input", "Table Input"),
    outport_(Port::OUTPORT, "volumehandle.output", "Table Output", false),
    enableProcessing_("enabled", "Enable", false),
    mode_("metric", "Metric"),
    uniformSpacing_("uniformSpacing", "Uniform Spacing", false),
    spacingX_("spacingX", "Spacing X", 1.0f, 0.0001f, 1000.f),
    spacingY_("spacingY", "Spacing Y", 1.0f, 0.0001f, 1000.f),
    spacingZ_("spacingZ", "Spacing Z", 1.0f, 0.0001f, 1000.f),
    spacingDisplay_("spacingDisplay", "Resulting Spacing", tgt::vec3(1.0f), tgt::vec3(0.0f), tgt::vec3(1000.f))
{
    addPort(inport_);
    addPort(outport_);

    addProperty(enableProcessing_);

    mode_.addOption("euclidean", "Euclidean");
    mode_.addOption("hypercube", "Hypercube");
    addProperty(mode_);

    addProperty(uniformSpacing_);

    spacingX_.setNumDecimals(5);
    spacingY_.setNumDecimals(5);
    spacingZ_.setNumDecimals(5);
    addProperty(spacingX_);
    addProperty(spacingY_);
    addProperty(spacingZ_);

    spacingDisplay_.setWidgetsEnabled(false);
    addProperty(spacingDisplay_);

    start_.onChange(
        CallMemberAction<NeighbourFinder>(this, &NeighbourFinder::process));
}

Processor* NeighbourFinder::create() const {
    return new NeighbourFinder();
}

void NeighbourFinder::process() {
    /*const VolumeBase* inputVolume = inport_.getData();

    if (!enableProcessing_.get()) {
        outport_.setData(inputVolume, false);
        spacingDisplay_.set(inputVolume->getSpacing());
        return;
    }


    VolumeBase* outputVolume =
        new VolumeDecoratorReplaceSpacing(inputVolume, spacing);
    outport_.setData(outputVolume);*/

}

void NeighbourFinder::spacingChanged(int dim) {

    if (!uniformSpacing_.get())
        return;

    if (dim == 0) {
        float xScale = spacingX_.get();
        spacingY_.set(xScale);
        spacingZ_.set(xScale);
    }
    else if (dim == 1) {
        float yScale = spacingY_.get();
        spacingX_.set(yScale);
        spacingZ_.set(yScale);
    }
    else if (dim == 2) {
        float zScale = spacingZ_.get();
        spacingX_.set(zScale);
        spacingY_.set(zScale);
    }
}

void NeighbourFinder::uniformScalingChanged() {
    if (uniformSpacing_.get())
        spacingChanged(0);
}

}   // namespace
