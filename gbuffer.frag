#version 150

in vec3 outVertPos;
in vec3 outVertNorm;
in vec3 outVertUV;

out vec4 outFrag[3];

void main() {
  outFrag[0] = vec4(outVertPos, 1);
  outFrag[1] = vec4(outVertNorm, 1);
  outFrag[2] = vec4(outVertUV, 1);
}
