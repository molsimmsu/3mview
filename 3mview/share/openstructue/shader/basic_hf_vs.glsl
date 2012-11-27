uniform bool lighting_flag;
uniform bool two_sided;
uniform bool occlusion_flag;

void main()
{    
  vec4 ground_color=0.2*gl_Color;
  vec4 sky_color=1.0*gl_Color;

  if(occlusion_flag) {
    ground_color.rgb*=gl_MultiTexCoord0.w;
    sky_color.rgb*=gl_MultiTexCoord0.w;
  }

  gl_Position = ftransform();

  // hemisphere lighting contribution
  vec3 ec_pos = vec3(gl_ModelViewMatrix* gl_Vertex);
  if(lighting_flag) {
    vec3 normal = vec3(0,0,1);
    if(dot(gl_Normal,gl_Normal)>0.001) {
      normal = normalize(gl_NormalMatrix * gl_Normal);
    }
    vec3 l_dir  = normalize(gl_LightSource[0].position.xyz); // assume directional vector
    float a  = 0.5+0.5*dot(normal,l_dir);
    gl_FrontColor.rgb = mix(ground_color, sky_color, a).rgb;
    gl_FrontColor.a = gl_Color.a;
    if(two_sided) {
      float a  = 0.5+0.5*dot(-normal,l_dir);
      gl_BackColor.rgb = mix(ground_color, sky_color, a).rgb;
      gl_BackColor.a = gl_Color.a;
    } else {
      gl_BackColor = gl_FrontColor;
    }
  } else {
    gl_FrontColor = gl_Color;
    gl_BackColor = gl_Color;
  }

  // for some reason, the fog and z coordinate are sign toggled...
  gl_FogFragCoord = -ec_pos.z;
}

