#version 150

in vec2 outVertBufferCoord;

out vec4 outFrag[3];

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

float linstep(float min, float max, float v) {
  return clamp((v-min) / (max-min), 0, 1);
}

float reduceLightBleeding(float p_max, float amount) {
  // Remove the [0, Amount] tail and linearly rescale (Amount, 1].
  return linstep(amount, 1, p_max);
}

float shadowContribution(vec3 dir, float depth, int index) {
  float M1 = texture(unifShadowMaps[index], dir).r;
  float M2 = texture(unifShadowMaps[index], dir).g;

  if (depth <= M1) return 0;

  float avg = M1;
  float var = M2 - M1 * M1;

  var = max(var, 0.0001);

  float pMax = var / (var + pow(depth - avg, 2));
  return 1 - reduceLightBleeding(pMax, 0.01);
}

void main() {
  vec2 sCoord = outVertBufferCoord;
  vec3 wPosition = texture(unifPositions, sCoord).xyz;
  vec3 wNormal = normalize(texture(unifNormals, sCoord).xyz);

  outFrag[0] = vec4(0,0,0,1); // lighting
  outFrag[1] = vec4(1,1,1,1); // shadow

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
    shadow = clamp(shadow,0,1);

    vec3 light = clamp(attenuation * reflected * lightColor, 0,1);
    outFrag[0] += vec4(light, 1.0);
    outFrag[1] -= vec4(shadow * light, 1.0);
  }
}
