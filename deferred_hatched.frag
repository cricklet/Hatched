#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;
uniform sampler2D unifDepths;

uniform sampler2D unifTilesTexture;
uniform int unifNumTiles;

uniform vec3 unifLightDir;

void main() {
  vec2 coord = outVertBufferCoord;
  vec3 position = texture(unifPositions, coord).xyz;
  vec2 uv = texture(unifUVs, coord).xy;
  vec3 normal = normalize(texture(unifNormals, coord).xyz);
  float depth = texture(unifDepths, coord).x;

  float light = 0.5 + 0.5 * dot(-normalize(unifLightDir), normalize(normal));

  float tileWeights[12]; // always must be greater than the number of tiles
  for (int i = 0; i < 12; i ++) {
    tileWeights[i] = 0;
  }
	
  // setting the corresponding tile weight
  float exact = (1 - light) * unifNumTiles;
  int index = int(exact);
  float weight = exact - index;
  //weight = pow(weight, 4);
	
  tileWeights[index] = 1 - weight;
  tileWeights[index + 1] = weight;
	
  outFragColor = vec4(0,0,0,1);
  for (int i = 0; i < unifNumTiles; i ++) {
    vec2 tileUV = uv;
    tileUV.x = mod(tileUV.x, 1);
    tileUV.x = tileUV.x * (1.0 / 6) + i * (1.0 / 6);
    outFragColor += tileWeights[i] * texture(unifTilesTexture, tileUV);
  }
}
