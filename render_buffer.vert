#version 150

in vec2 inVertPosition;
in vec2 inVertBufferCoord;

out vec2 outVertBufferCoord;

uniform float unifScale = 1;
uniform float unifOffsetX = 0;
uniform float unifOffsetY = 0;

void main () {
  outVertBufferCoord = inVertBufferCoord;
  vec2 pos = inVertPosition;
  pos *= unifScale;
  pos.x += unifOffsetX;
  pos.y += unifOffsetY;
  gl_Position = vec4(pos, 0,1);
}
