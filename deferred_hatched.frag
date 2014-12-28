#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform mat4 unifViewTrans;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;

uniform sampler2D unifBuffer;

int NUM_HATCHES = 6;
uniform sampler2D unifHatch0;
uniform sampler2D unifHatch1;
uniform sampler2D unifHatch2;
uniform sampler2D unifHatch3;
uniform sampler2D unifHatch4;
uniform sampler2D unifHatch5;

uniform vec3 unifLightDir;

void draw(vec2 uv, int tLevel, float weight) {
  uv *= 2;
  if (tLevel == 0) outFragColor += weight * texture(unifHatch0, uv);
  else if (tLevel == 1) outFragColor += weight * texture(unifHatch1, uv);
  else if (tLevel == 2) outFragColor += weight * texture(unifHatch2, uv);
  else if (tLevel == 3) outFragColor += weight * texture(unifHatch3, uv);
  else if (tLevel == 4) outFragColor += weight * texture(unifHatch4, uv);
  else if (tLevel == 5) outFragColor += weight * texture(unifHatch5, uv);
  else return;
}

void drawHatching(float light, vec2 uv) {
  ////////////////////////////
  // calculate the tone levels
  float tLevel = (1 - light) * NUM_HATCHES - 2;

  int tLevel0 = int(tLevel);
  float tWeight0 = tLevel - tLevel0;

  int tLevel1 = int(tLevel) + 1;
  float tWeight1 = 1 - tWeight0;

  tLevel0 = clamp(tLevel0, 0, NUM_HATCHES - 1);
  tLevel1 = clamp(tLevel1, 0, NUM_HATCHES - 1);

  outFragColor = vec4(0,0,0,1);

  {
    draw(uv, tLevel0 - 1, 0);
    draw(uv, tLevel1 + 1, 0);
  }

  draw(uv, tLevel0, tWeight0);
  draw(uv, tLevel1, tWeight1);
}

void main() {
  vec2 sCoord = outVertBufferCoord;

  vec2 uv = texture(unifUVs, sCoord).xy;

  vec3 wPos = texture(unifPositions, sCoord).xyz;
  vec3 wNorm = texture(unifNormals, sCoord).xyz;
  vec3 vPos = vec3(unifViewTrans * vec4(wPos, 1));
  float vDepth = length(vPos);

  float light = 0.5 + 0.5 * dot(-normalize(unifLightDir), normalize(wNorm));
  light = int(light * 16) / 16.0;
  
  float ssao = texture(unifBuffer, sCoord).x;
  float hardSSAO = 1 - 4 * (1 - ssao);
  float lightSSAO = 1 - 2 * (1 - ssao);

  drawHatching(1 - clamp(2 - hardSSAO - light, 0,1), uv);

  //outFragColor = vec4(1,1,1,1);
  //outFragColor -= vec4(1 - lightSSAO, 1 - lightSSAO, 1 - lightSSAO, 1);
  outFragColor -= vec4(1 - light, 1 - light, 1 - light, 1);
}
