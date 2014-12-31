#version 150

in vec3 outVertViewPos;
out vec4 outFrag;

void main() {
  float depth = length(outVertViewPos) + 0.05;
  outFrag = vec4(depth, depth * depth, 0,1);
}
