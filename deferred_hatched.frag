#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform mat4 unifViewTrans;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;

uniform sampler2D unifLightBuffer;
uniform sampler2D unifShadowBuffer;
uniform sampler2D unifSSAOBuffer;

int NUM_HATCHES = 6;
uniform sampler2D unifHatch0;
uniform sampler2D unifHatch1;
uniform sampler2D unifHatch2;
uniform sampler2D unifHatch3;
uniform sampler2D unifHatch4;
uniform sampler2D unifHatch5;

vec3 draw(vec2 uv, int tLevel, float weight) {
  if (tLevel == -1) return weight * vec3(1,1,1);
  else if (tLevel == 0) return weight * texture(unifHatch0, uv).rgb;
  else if (tLevel == 1) return weight * texture(unifHatch1, uv).rgb;
  else if (tLevel == 2) return weight * texture(unifHatch2, uv).rgb;
  else if (tLevel == 3) return weight * texture(unifHatch3, uv).rgb;
  else if (tLevel == 4) return weight * texture(unifHatch4, uv).rgb;
  else if (tLevel == 5) return weight * texture(unifHatch5, uv).rgb;
  else return vec3(0,0,0);
}

vec3 drawHatching(vec3 light, vec2 uv) {
  ////////////////////////////
  // calculate the tone levels
  float lightVal = max(max(light.r, light.g), light.b);
  float tLevel = (1 - lightVal) * NUM_HATCHES - 2;

  int tLevel0 = int(tLevel);
  float tWeight0 = tLevel - tLevel0;

  int tLevel1 = int(tLevel) + 1;
  float tWeight1 = 1 - tWeight0;

  tLevel0 = clamp(tLevel0, 0, NUM_HATCHES - 1);
  tLevel1 = clamp(tLevel1, 0, NUM_HATCHES - 1);

  vec3 result = vec3(0,0,0);

  {
    result += draw(uv, tLevel0 - 1, 0);
    result += draw(uv, tLevel1 + 1, 0);
  }

  result += draw(uv, tLevel0, tWeight0);
  result += draw(uv, tLevel1, tWeight1);

  return result * light;
}

void main() {
  vec2 sCoord = outVertBufferCoord;

  vec2 uv = texture(unifUVs, sCoord).xy;

  vec3 wPos = texture(unifPositions, sCoord).xyz;
  vec3 wNorm = texture(unifNormals, sCoord).xyz;
  vec3 vPos = vec3(unifViewTrans * vec4(wPos, 1));
  float vDepth = length(vPos);

  vec3 light = vec3(texture(unifLightBuffer, sCoord));
  vec3 shadow = vec3(texture(unifShadowBuffer, sCoord));
  vec3 ssao = vec3(texture(unifSSAOBuffer, sCoord));

  vec3 result = light - (1 - shadow) - (1 - ssao);
  vec3 hatching = drawHatching(shadow, uv);

  //outFragColor = vec4(result, 1);
  outFragColor = vec4(result, 1);
}
