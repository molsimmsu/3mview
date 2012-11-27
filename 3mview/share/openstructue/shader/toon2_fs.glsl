uniform bool lighting_flag;
uniform bool two_sided_flag;
uniform bool fog_flag;

void DirectionalLight(in vec3 normal,
                      in float shin,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 spec)
{
  float n_vp = max(0.0, dot(normal, normalize(vec3(gl_LightSource[0].position))));
  if (n_vp>0.6) {
    n_vp=0.9;
  } else if (n_vp>0.5) {
    n_vp=0.7;
  } else if (n_vp>0.05) {
    n_vp=0.1;
  } else {
    n_vp=0.0;
  }

  float n_hv = max(0.0, dot(normal, vec3(gl_LightSource[0].halfVector)));
  float pf = shin>0.0 && n_vp>0.0 ? (pow(n_hv,shin)>0.8 ? 1.0 : 0.0) : 0.0;

  ambient  += gl_LightSource[0].ambient;
  diffuse  += gl_LightSource[0].diffuse*n_vp;
  spec     += gl_LightSource[0].specular*pf;
}

void main()
{
  vec4 amb = vec4(0.0);
  vec4 diff = vec4(0.0);
  vec4 spec = vec4(0.0);
  vec4 color = vec4(0.0);
  vec3 normal = normalize(gl_TexCoord[2].stp);

  DirectionalLight(normal, gl_FrontMaterial.shininess, amb, diff, spec);

  color = (gl_FrontLightModelProduct.sceneColor + 
           amb*gl_Color + diff*gl_Color + spec*gl_Color);

  if(two_sided_flag) {
    amb=vec4(0.0);
    diff=vec4(0.0);
    spec=vec4(0.0);
    DirectionalLight(-normal, gl_FrontMaterial.shininess, amb, diff, spec);
    color += (gl_BackLightModelProduct.sceneColor + 
              amb*gl_Color + diff*gl_Color + spec*gl_Color);
  }

  gl_FragColor = clamp(color,0.0,1.0);
  gl_FragColor.a = gl_Color.a;
}
