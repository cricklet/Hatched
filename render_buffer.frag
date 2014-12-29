#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;
uniform sampler2D unifBuffer;

void main() {
  vec2 coord = outVertBufferCoord;
  outFragColor = texture(unifBuffer, coord);
}
