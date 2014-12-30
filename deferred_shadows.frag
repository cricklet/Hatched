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
    if (i == 0) {
      float shadowDist = texture(unifShadowMap, wDir).r + 0.01;
      if (shadowDist < wDist) {
	shadow = 1;
      }
    }

    vec3 light = (1 - shadow) * attenuation * reflected * lightColor;
    outFragColor += vec4(light, 1.0);
  }
}
