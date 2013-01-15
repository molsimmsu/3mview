
#include "modules/mod_sampler2d.frag"
#include "modules/mod_filtering.frag"

uniform sampler2D colorTex_;
uniform sampler2D depthTex_;
uniform TextureParameters textureParameters_;

uniform float saturation_;

void main() {
    // lookup input color and depth value (see mod_sampler2d.frag)
    vec4 color = textureLookup2Dscreen(colorTex_, textureParameters_, gl_FragCoord.xy);
    float depth = textureLookup2Dscreen(depthTex_, textureParameters_, gl_FragCoord.xy).z;

    // compute gray value (see mod_filtering.frag) and pass-through depth value
    FragData0 = rgbToGrayScaleSaturated(color, saturation_);
    gl_FragDepth = depth;
}
