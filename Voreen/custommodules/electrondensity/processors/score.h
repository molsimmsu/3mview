#ifndef VRN_SCORE_H
#define VRN_SCORE_H

#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/ports/volumeport.h"

namespace voreen {

class Score : public Processor {
public:
    Score();
    ~Score();
    virtual Processor* create() const;

    virtual std::string getClassName() const      { return "Score";     }
    virtual std::string getCategory() const       { return "Form Analysis"; }
    virtual CodeState getCodeState() const        { return CODE_STATE_EXPERIMENTAL;   }
    virtual bool usesExpensiveComputation() const { return true; }

protected:
    virtual void setDescriptions() {
        setDescription("Calculates the difference between two volumes.");
    }

    virtual void process() {}

    double calculateScore(const VolumeBase* firstVolume, const VolumeBase* secondVolume);
    
    void calcAndShowScore();

    tgt::mat4 computeConversionMatrix(const VolumeBase* originVolume, const VolumeBase* destinationVolume) const;

private:
    /**
     * Combines the input volumes and writes the result to combinedVolume (which is assumed to be already created),
     * by transforming the input volume's coordinates systems to the coordinates system of the combined volume.
     */
    void combineVolumes(Volume* combinedVolume, const VolumeBase* firstVolume,
        const VolumeBase* secondVolume) const;

    /**
     * Combines the input volumes and writes the result to combinedVolume (which is assumed to be already created),
     * without coordinate transformation. This is much faster than the transformation-based combination,
     * but only possible for volumes that share a common grid in world space.
     */
    void combineVolumesOnCommonGrid(Volume* combinedVolume, const VolumeBase* firstVolume,
        const VolumeBase* secondVolume) const;

    /// Creates a combined (empty) volume from the two input volumes in world space,
    /// or 0 in case the combined volume could not be created due to bad allocation.
    Volume* createCombinedVolume(const VolumeBase* refVolume, const VolumeBase* secondVolume) const;

    StringOptionProperty    distanceType_;
    StringOptionProperty 	filteringMode_;
    StringProperty         	output_;
    ButtonProperty         	calcButton_;
    
    VolumePort inport1_;
    VolumePort inport2_;

    static const std::string loggerCat_; ///< category used in logging
};


} // namespace

#endif // VRN_Score_H
