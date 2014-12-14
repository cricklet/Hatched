#version 150

in vec2 inVertPosition;
in vec2 inVertBufferCoord;

out vec2 outVertBufferCoord;

void main () {
  outVertBufferCoord = inVertBufferCoord;
  gl_Position = vec4(inVertPosition, 0,1);
}