#version 150

out vec4 outFragColor;
uniform vec3 unifColor;

void main() {
  outFragColor = vec4(unifColor, 1.0);
  //outFragColor = vec4(1,0,0, 1.0);
}
