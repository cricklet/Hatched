#version 150

in vec3 outVertColor;
out vec4 outFragColor;

void main() {
  outFragColor = vec4(outVertColor, 1.0);
}
