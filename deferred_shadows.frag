#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

const int MAX_LIGHTS = 100;
uniform vec3 unifLightPositions[MAX_LIGHTS];
uniform vec3 unifLightConstants[MAX_LIGHTS]; // constant, linear, squared
uniform vec3 unifLightColors[MAX_LIGHTS];
uniform int unifNumLights;

const int MAX_SHADOW_MAPS = 8;
uniform samplerCube unifShadowMaps[MAX_SHADOW_MAPS];
uniform int unifNumShadowMaps;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;

float shadowContribution(vec3 dir, float depth, int index) {
  float M1 = texture(unifShadowMaps[index], dir).r;
  float M2 = texture(unifShadowMaps[index], dir).g;

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

    float shadow = 0;
    if (i < unifNumShadowMaps) {
      shadow = shadowContribution(wDir, wDist, i);
    }

    float light = (1 - shadow) * attenuation * reflected;
    light = clamp(light, 0,1);
    outFragColor += vec4(light * lightColor, 1.0);
  }
}
