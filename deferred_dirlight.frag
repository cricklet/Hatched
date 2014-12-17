#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;
uniform sampler2D unifDepths;

uniform vec3 unifLightDir;

void main() {
  vec2 coord = outVertBufferCoord;
  vec3 position = texture(unifPositions, coord).xyz;
  vec3 normal = normalize(texture(unifNormals, coord).xyz);
  float depth = texture(unifDepths, coord).x;

  float light = 0.5 + 0.5 * dot(-normalize(unifLightDir), normalize(normal));
  outFragColor = vec4(light, light, light, 1.0);
}
