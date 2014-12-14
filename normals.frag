#version 150

in vec3 outVertNorm;
out vec4 outFragColor;

void main() {
  float depth = gl_FragCoord.z / gl_FragCoord.w;
  outFragColor = vec4(outVertNorm, depth);
}
