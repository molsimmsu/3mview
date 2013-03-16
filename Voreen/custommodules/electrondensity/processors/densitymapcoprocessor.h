#ifndef VRN_DENSITYMAPCOPROCESSOR_H
#define VRN_DENSITYMAPCOPROCESSOR_H

#include "densitymapcollectionsource.h"

using namespace voreen;
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
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; }
    
    virtual void process() {}
    
    virtual void updateSelection() {}
    
protected:
    virtual void setDescriptions() {
        setDescription("Base co-processor for density maps operating");
    }

	const VolumeCollection* getInputVolumeCollection() const;
	DensityMapCollectionSource* getSourceProcessor() const;
	
	static const std::string loggerCat_;

private:
	CoProcessorPort inport_;
};

#endif // VRN_DENSITYMAPCOPROCESSOR_H
