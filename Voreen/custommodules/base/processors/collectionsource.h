#ifndef VRN_COLLECTIONSOURCE_H
#define VRN_COLLECTIONSOURCE_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/processors/processorwidgetfactory.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/properties/filedialogproperty.h"
using namespace voreen;

template <typename CollectionBase>
class CollectionSource : public Processor {

public:
    CollectionSource();
    virtual ~CollectionSource();
    virtual Processor* create() const;

    virtual std::string getClassName() const  { return "CollectionSource"; }
    virtual std::string getCategory() const   { return "Input";            }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE;  }

    virtual void invalidate(int inv = INVALID_RESULT);

    void setCollection(CollectionBase* collection, bool owner = false);

    CollectionBase* getCollection() const;
    
    CollectionBase* getSelectedCollection() const;

protected:
    virtual void setDescriptions() {
        setDescription("Loads a collection of a given type and allows for selection of active elements");
    }

    virtual void process();
    virtual void initialize() throw (tgt::Exception);

    /// The volume port the loaded data set is written to.
    GenericPort<CollectionBase> collectionPort_;
    CoProcessorPort coProcessorPort_;

    static const std::string loggerCat_;
};

#endif
