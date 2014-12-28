#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform mat4 unifViewTrans;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;

uniform sampler2D unifBuffer;

uniform sampler2D unifTilesTexture;
uniform int unifNumTones;
uniform int unifNumMips;

uniform vec3 unifLightDir;

void draw(vec2 uv, float mLevel, float tLevel, float weight) {
  if (mLevel < 0 || tLevel < 0) return;
  
  vec2 tilesUV = mod(2 * uv, 1);
  tilesUV.x /= unifNumTones;
  tilesUV.y /= 2;
  tilesUV /= pow(2, mLevel);
  tilesUV.y += 1 - pow(0.5, mLevel);
  tilesUV.x += tLevel / unifNumTones;
  outFragColor += weight * texture(unifTilesTexture, tilesUV);
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
  float lightSSAO = 1 - 1 * (1 - ssao);

  ///////////////////////////
  // calculate the mip levels
  float mipNearestDepth = 8;

  float mDepth = vDepth / mipNearestDepth;
  float mLevel = clamp(log(mDepth) / log(2), 0, unifNumMips - 1);

  int mLevel0 = int(mLevel);
  int mLevel1 = mLevel0 + 1;

  float mWeight1 = mLevel - mLevel0;
  mWeight1 = clamp(2 * mWeight1, 0, 1);
  float mWeight0 = 1 - mWeight1;

  ////////////////////////////
  // calculate the tone levels
  float tLevel = (1 - hardSSAO) * unifNumTones - 2;

  int tLevel0 = int(tLevel);
  float tWeight0 = tLevel - tLevel0;

  int tLevel1 = int(tLevel) + 1;
  float tWeight1 = 1 - tWeight0;

  tLevel0 = clamp(tLevel0, 0, unifNumTones - 1);
  tLevel1 = clamp(tLevel1, 0, unifNumTones - 1);

  outFragColor = vec4(0,0,0,1);

  {
    draw(uv, mLevel0, tLevel0 - 1, 0);
    draw(uv, mLevel1, tLevel0 - 1, 0);
    draw(uv, mLevel0 - 1, tLevel0, 0);
    draw(uv, mLevel0 - 1, tLevel1, 0);
  }

  draw(uv, mLevel0, tLevel0, mWeight0 * tWeight0);
  draw(uv, mLevel1, tLevel0, mWeight1 * tWeight0);
  draw(uv, mLevel0, tLevel1, mWeight0 * tWeight1);
  draw(uv, mLevel1, tLevel1, mWeight1 * tWeight1);

  //outFragColor = vec4(1,1,1,1);
  //outFragColor -= vec4(1 - lightSSAO, 1 - lightSSAO, 1 - lightSSAO, 1);
  outFragColor -= vec4(1 - light, 1 - light, 1 - light, 1);
}
