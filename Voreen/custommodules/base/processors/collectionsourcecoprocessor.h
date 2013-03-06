#ifndef VRN_DENSITYMAPCOPROCESSOR_H
#define VRN_DENSITYMAPCOPROCESSOR_H

#include "densitymapcollectionsource.h"

namespace voreen {
/*
 * Base co-processor for handling density maps
 */
class DensityMapCoProcessor : virtual public Processor {
public:
    DensityMapCoProcessor();
    virtual ~DensityMapCoProcessor() {}
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "DensityMapCoProcessor";       }
    virtual std::string getCategory() const  { return "Density Map Co-Processor";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
    virtual void process() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Base co-processor for density maps operating");
    }

	const VolumeCollection* getInputVolumeCollection() const;
	DensityMapCollectionSource* getSourceProcessor() const;

private:
	CoProcessorPort inport_;
};

} // namespace

#endif // VRN_DENSITYMAPCOPROCESSOR_H
