void main()
{
  // transformed position
  gl_Position = ftransform();

  vec4 ec_Pos = gl_ModelViewMatrix* gl_Vertex;
  // for some reason, the fog and z coordinate are sign toggled...
  gl_FogFragCoord = -ec_Pos.z;

  // since a directional light is used, the position is not needed
  // and only the normal is passed on
  gl_TexCoord[2].stp=normalize(gl_NormalMatrix * gl_Normal);

  // default tex coord
  gl_TexCoord[0] = gl_MultiTexCoord0;

  gl_FrontColor=gl_Color;
  gl_BackColor=gl_Color;
}

