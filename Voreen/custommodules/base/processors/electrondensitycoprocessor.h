#ifndef VRN_DENSITYMAPCOPROCESSOR_H
#define VRN_DENSITYMAPCOPROCESSOR_H

#include "densitymapcollectionsource.h"

namespace voreen {

/*
 * Base co-processor for handling density maps
 */
class DensityMapCoProcessor : public Processor {
public:
    DensityMapCoProcessor();
    virtual ~DensityMapCoProcessor() {}
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "DensityMapCoProcessor";       }
    virtual std::string getCategory() const  { return "Density Map Co-Processor";      }
    virtual CodeState getCodeState() const   { return CODE_STATE_EXPERIMENTAL; }
    
protected:
	const VolumeCollection* getInputVolumeCollection() const;

private:
    DensityMapCollectionSource* getSourceProcessor() const;
	
	CoProcessorPort inport_;

};

} // namespace

#endif // VRN_DENSITYMAPCOPROCESSOR_H
