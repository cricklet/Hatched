#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform mat4 unifViewTrans;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;
uniform sampler2D unifDepths;

uniform sampler2D unifTilesTexture;
uniform int unifNumTones;
uniform int unifNumMips;

uniform vec3 unifLightDir;

void draw(vec2 uv, float mLevel, float tLevel, float weight) {
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

  vec3 wPos = texture(unifPositions, sCoord).xyz;
  vec3 wNorm = normalize(texture(unifNormals, sCoord).xyz);
  vec3 vPos = vec3(unifViewTrans * vec4(wPos, 1));
  float vDepth = length(vPos);

  vec2 uv = texture(unifUVs, sCoord).xy;
  float light = 0.5 + 0.5 * dot(-normalize(unifLightDir), normalize(wNorm));

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
  float toneLevel = (1 - light) * unifNumTones;

  int toneLevel0 = int(toneLevel);
  float toneWeight0 = toneLevel - toneLevel0;

  int toneLevel1 = int(toneLevel) + 1;
  float toneWeight1 = 1 - toneWeight0;

  toneLevel0 = clamp(toneLevel0, 0, unifNumTones - 1);
  toneLevel1 = clamp(toneLevel1, 0, unifNumTones - 1);

  outFragColor = vec4(0,0,0,1);

  draw(uv, mLevel0, toneLevel0, mWeight0 * toneWeight0);
  draw(uv, mLevel1, toneLevel0, mWeight1 * toneWeight0);
  draw(uv, mLevel0, toneLevel1, mWeight0 * toneWeight1);
  draw(uv, mLevel1, toneLevel1, mWeight1 * toneWeight1);
}
