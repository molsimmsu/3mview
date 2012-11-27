uniform bool fog_flag;

void main()
{
  if(fog_flag) {
    float fog = clamp((gl_Fog.end-gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0);
    gl_FragColor.rgb = mix(gl_Fog.color.rgb, gl_Color.rgb, fog);
    gl_FragColor.a = gl_Color.a;
  } else {
    gl_FragColor = gl_Color;
  }
}

