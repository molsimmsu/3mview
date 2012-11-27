void main()
{
  gl_FragColor.rgb=normalize(gl_TexCoord[0]).stp*0.5+0.5;
}

