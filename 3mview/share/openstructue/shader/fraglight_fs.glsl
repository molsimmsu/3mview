uniform bool lighting_flag;
uniform bool two_sided_flag;
uniform bool fog_flag;
uniform sampler2D depth_map;
uniform int depth_mode;
uniform bool tex_flag;
uniform sampler2D tex_map;

// copy from basic_fl_vs !
bool DirectionalLight(in vec3 normal,
                      in float shin,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular,
		      inout bool lflag)
{
  float n_vp = max(0.0, dot(normal, normalize(gl_LightSource[0].position.xyz)));

  float pf = 0.0;
  lflag = n_vp>0.0;
  if(n_vp>0.0 && shin>0.0) {
    float n_hv = max(0.0, dot(normal, normalize(gl_LightSource[0].halfVector.xyz)));
    pf=pow(n_hv, shin);
  }

  ambient  += gl_LightSource[0].ambient;
  diffuse  += gl_LightSource[0].diffuse * n_vp;
  specular += gl_LightSource[0].specular * pf;

  return true;
}

void main()
{
  bool lflag=false;

  vec4 color = gl_Color;

  if(tex_flag) {
    color.rgb = texture2D(tex_map,gl_TexCoord[0].st).rgb;
  }

  if(lighting_flag) {
    vec3 normal = normalize(gl_TexCoord[2].stp);

    vec4 amb = vec4(0.0);
    vec4 diff = vec4(0.0);
    vec4 spec = vec4(0.0);

    if(DirectionalLight(normal, gl_FrontMaterial.shininess, amb, diff, spec, lflag)) {

      color  = gl_FrontLightModelProduct.sceneColor  +
               (amb  * gl_FrontMaterial.ambient * color) +
               (diff * gl_FrontMaterial.diffuse * color) +
               (spec * gl_FrontMaterial.specular);
    } else {
      bool dummy;
      DirectionalLight(-normal, gl_BackMaterial.shininess, amb, diff, spec, dummy);

      color = gl_BackLightModelProduct.sceneColor  +
              (amb  * gl_BackMaterial.ambient * color) +
              (diff * gl_BackMaterial.diffuse * color) +
              (spec * gl_BackMaterial.specular);
    }
  }

  gl_FragColor = color;

  float fog = fog_flag ? clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0) : 1.0;
  gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_FragColor.rgb, fog);
  gl_FragColor.a = gl_Color.a;
}
