precision mediump float;
precision mediump int;

attribute vec4 vPosition;
attribute vec2 vTexCoord;
varying vec2 texCoord;
uniform vec4 color;
uniform int istext;

void main() {
  if (istext == 1) {
    gl_Position = vec4(vPosition.xy, 0.0, 1.0);
  }
  else {
    gl_Position = vPosition;
  }

  texCoord = vTexCoord;
}
