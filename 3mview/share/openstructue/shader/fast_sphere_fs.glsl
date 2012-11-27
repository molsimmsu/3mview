uniform bool lighting_flag;
uniform bool two_sided_flag;
uniform bool fog_flag;
uniform bool write_normals;
uniform bool use_hemimodel;

// gl_TexCoord[0] is from gl_MultiTexCoord0, which in turn
// is custom crafted in the fast sphere prep routine

// copy from basic_fl_vs !
bool DirectionalLight(in vec3 normal,
                      in float shin,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular)
{
  float n_vp = max(0.0, dot(normal, normalize(vec3(gl_LightSource[0].position))));
  float n_hv = max(0.0, dot(normal, vec3(gl_LightSource[0].halfVector)));
  float pf = n_vp>0.0 ? pow(n_vp, shin) : 0.0;

  ambient  += gl_LightSource[0].ambient;
  diffuse  += gl_LightSource[0].diffuse*n_vp;
  specular += gl_LightSource[0].specular * pf;
  return true;
}

void main()
{
  float xx = (gl_TexCoord[0].x-0.5)*2.0;
  float yy = (gl_TexCoord[0].y-0.5)*2.0;
  float xxyy = xx*xx+yy*yy;
  if(xxyy>1.0) discard;
  
  float zz = sqrt(1.0-clamp(xx*xx+yy*yy,0.0,1.0));
  vec3 normal=vec3(xx,yy,zz);

  float z1 = gl_TexCoord[0].z*zz;
  float z2 = gl_TexCoord[0].w*zz;

  gl_FragDepth = gl_FragCoord.z+z2;

  if(write_normals) {
    gl_FragColor.rgb=normal*0.5+0.5;
    return;
  }

  vec4 color = vec4(0.0);
  if(use_hemimodel) {
    color = gl_Color;
  } else {
    vec4 amb = vec4(0.0);
    vec4 diff = vec4(0.0);
    vec4 spec = vec4(0.0);

    DirectionalLight(normal, gl_FrontMaterial.shininess, amb, diff, spec);
    color  = gl_FrontLightModelProduct.sceneColor  +
             (amb  * gl_FrontMaterial.ambient * gl_Color) +
             (diff * gl_FrontMaterial.diffuse * gl_Color) +
             (spec * gl_FrontMaterial.specular);
  }

  if(fog_flag) {
    float fog = clamp((gl_Fog.end-(gl_FogFragCoord+z1)) * gl_Fog.scale, 0.0, 1.0);
    color.rgb = mix(gl_Fog.color.rgb, color.rgb, fog);
  }
  gl_FragColor.rgb = color.rgb;
  gl_FragColor.a = 1.0;

}
