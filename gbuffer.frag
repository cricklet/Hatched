#version 150

in vec3 outVertPos;
in vec3 outVertNorm;
in vec2 outVertUV;

out vec4 outFrag[3];

void main() {
  outFrag[0] = vec4(outVertPos, 1);
  outFrag[1] = vec4(outVertNorm, 0);
  outFrag[2] = vec4(outVertUV, 0,0);
}
