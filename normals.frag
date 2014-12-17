#version 150

in vec3 outVertNorm;
out vec4 outFragColor;

void main() {
  vec3 norm = normalize(outVertNorm);
  norm.z = norm.z / gl_FragCoord.w;
  norm = 0.5 * (norm + vec3(1,1,1));
  outFragColor = vec4(norm, 1);
}
