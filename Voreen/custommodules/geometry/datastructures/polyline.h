#ifndef VRN_POLYLINE_H
#define VRN_POLYLINE_H

#include "voreen/core/io/serialization/serializable.h"
using namespace voreen;

#include "tgt/vector.h"

class Polyline : public Serializable {
public:
    Polyline();
    Polyline(OBMol* mol);
    virtual ~Polyline() {}
    
    /**
     * Returns the name of this class as a string.
     * Necessary due to the lack of code reflection in C++.
     *
     * This method is expected to be re-implemented by each concrete subclass.
     */
    virtual std::string getClassName() const { return "Polyline"; };

    /**
     * Returns a copy of the Polyline object.
     */
    virtual Polyline* clone() const;
    
    /**
     * TODO Implementation of the Serializable interface.
     */
    virtual void serialize(XmlSerializer& s) const {}

    /**
     * TODO Implementation of the Serializable interface.
     */
    virtual void deserialize(XmlDeserializer& s) {}
    
private:

};

#endif // VRN_POLYLINE_H
