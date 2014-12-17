#version 150

in vec3 outVertNorm;
in vec3 outVertPos;

out vec4 outFrag[2];

void main() {
  outFrag[0] = vec4(outVertPos, 1);
  outFrag[1] = vec4(outVertNorm, 1);
}
