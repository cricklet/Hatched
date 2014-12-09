#version 150

out vec4 outFragColor;
uniform vec3 inColor;

void main() {
  outFragColor = vec4(inColor, 1.0);
}
