precision mediump float;
precision mediump int;

varying vec2 texCoord;
uniform sampler2D tex;
uniform vec4 color;
uniform int istext;

void main() {
  if (istext == 1) {
    gl_FragColor = vec4(color.rg, 1.0, texture2D(tex, texCoord).a);
  }
  else  {
    gl_FragColor = vec4(color.rgb, 1.0);
  }
}
