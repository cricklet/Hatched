#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform vec3 unifLightPositions[100];
uniform vec3 unifLightConstants[100]; // constant, linear, squared
uniform vec3 unifLightColors[100];
uniform int unifNumLights;

uniform samplerCube unifShadowMap; // for the first light

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;

float shadowContribution(vec3 dir, float depth) {
  float M1 = texture(unifShadowMap, dir).r;
  float M2 = texture(unifShadowMap, dir).g;

  if (depth <= M1) return 0;

  float avg = M1;
  float var = M2 - M1 * M1;

  var = max(var, 0.0001);

  return 1 - var / (var + pow(depth - avg, 2));
}

void main() {
  vec2 sCoord = outVertBufferCoord;
  vec3 wPosition = texture(unifPositions, sCoord).xyz;
  vec3 wNormal = normalize(texture(unifNormals, sCoord).xyz);

  outFragColor = vec4(0.1,0.1,0.1,1);

  for (int i = 0; i < unifNumLights; i ++) {
    vec3 wLightPos = unifLightPositions[i];

    vec3 lightColor = unifLightColors[i];
    vec3 lightConst = unifLightConstants[i];
    float constant = lightConst.r;
    float linear   = lightConst.g;
    float squared  = lightConst.b;

    vec3 wDir = wPosition - wLightPos;
    float wDist = length(wDir);
    float attenuation = 1.0 / (constant + linear * wDist + squared * wDist * wDist);
    float reflected = dot(-normalize(wDir), wNormal);

    float shadow = shadowContribution(wDir, wDist);
    float light = (1 - shadow) * attenuation * reflected;
    light = clamp(light, 0,1);
    outFragColor += vec4(light * lightColor, 1.0);
  }
}
