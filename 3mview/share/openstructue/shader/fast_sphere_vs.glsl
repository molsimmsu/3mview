bool use_hemimodel;

void main()
{
  // transformed position
  gl_Position = ftransform();

  vec4 ec_Pos = gl_ModelViewMatrix* gl_Vertex;
  // for some reason, the fog and z coordinate are sign toggled...
  gl_FogFragCoord = -ec_Pos.z;

  gl_TexCoord[0]=gl_MultiTexCoord0;

  if(use_hemimodel) {
    vec4 ground_color=0.2*gl_Color;
    vec4 sky_color=1.0*gl_Color;

    // hemisphere lighting contribution
    vec3 ec_pos = vec3(gl_ModelViewMatrix* gl_Vertex);
    vec3 normal = vec3(0,0,1);
    if(dot(gl_Normal,gl_Normal)>0.001) {
      normal = normalize(gl_NormalMatrix * gl_Normal);
    }
    vec3 l_dir  = normalize(gl_LightSource[0].position.xyz); // assume directional vector
    float a  = 0.5+0.5*dot(normal,l_dir);
    gl_FrontColor.rgb = mix(ground_color, sky_color, a).rgb;
    gl_FrontColor.a = gl_Color.a;
    gl_BackColor = gl_FrontColor;
  } else {
    gl_FrontColor=gl_Color;
    gl_BackColor=gl_Color;
  }
}
