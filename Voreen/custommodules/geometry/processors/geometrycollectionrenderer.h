#ifndef VRN_GeometryCollectionRenderer_H
#define VRN_GeometryCollectionRenderer_H

#include "../ports/geometrycollectionport.h"

#include "voreen/core/processors/geometryrendererbase.h"

#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/vectorproperty.h"

namespace voreen {

/**
 * Basic processor for rendering arbitrary geometry, simply taking
 * a Geometry object through its inport und calling render() on it.
 */
class VRN_CORE_API GeometryCollectionRenderer : public GeometryRendererBase {
public:
    GeometryCollectionRenderer();
    virtual Processor* create() const;

    virtual std::string getCategory() const   { return "Geometry";         }
    virtual std::string getClassName() const  { return "GeometryCollectionRenderer"; }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE;  }

    virtual bool isReady() const;

    /**
     * Calls render() on the Geometry object.
     */
    virtual void render();

protected:
    virtual void setDescriptions() {
        setDescription("Renders the passed geometry object with adjustable polygon mode and lighting settings. Additionally, a texture passed trough the render inport can be mapped onto the geometry (texture coordinates have to be set by the geometry object).");
    }

    virtual void updatePropertyVisibilities();

    GeometryCollectionPort inport_;
    RenderPort texPort_;
    GLEnumOptionProperty polygonMode_;

    BoolProperty mapTexture_;
    IntOptionProperty textureMode_;

    BoolProperty enableLighting_;
    FloatVec4Property lightPosition_;       ///< The position of the light source in world coordinates
    FloatVec4Property lightAmbient_;        ///< The light source's ambient color
    FloatVec4Property lightDiffuse_;        ///< The light source's diffuse color
    FloatVec4Property lightSpecular_;       ///< The light source's specular color
    FloatProperty materialShininess_;   /// The material's specular exponent

};

}

#endif // VRN_GeometryCollectionRenderer_H

