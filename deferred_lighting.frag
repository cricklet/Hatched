#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform vec3 unifLightPositions[100];
uniform vec3 unifLightConstants[100]; // constant, linear, squared
uniform vec3 unifLightColors[100];
uniform int unifNumLights;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;

void main() {
  vec2 coord = outVertBufferCoord;
  vec3 position = texture(unifPositions, coord).xyz;
  vec3 normal = normalize(texture(unifNormals, coord).xyz);

  outFragColor = vec4(0.1,0.1,0.1,1);

  for (int i = 0; i < unifNumLights; i ++) {
    vec3 lightPos = unifLightPositions[i];
    vec3 lightColor = unifLightColors[i];
    vec3 lightConst = unifLightConstants[i];
    float constant = lightConst.r;
    float linear   = lightConst.g;
    float squared  = lightConst.b;

    vec3 dir = position - lightPos;
    float dist = length(dir);
    float sqDist = dist * dist;
    float attenuation = 1.0 / (constant + linear * dist + squared * sqDist);
    float reflected = dot(-normalize(dir), normal);
    vec3 light = attenuation * reflected * lightColor;
    outFragColor += vec4(light, 1.0);
  }
}
