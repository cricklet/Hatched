#version 150

in vec2 outVertUV;
in float outVertLighting;

out vec4 outFragColor;

uniform vec3 unifColor;
uniform sampler2D unifTilesTexture;
uniform int unifNumTiles;

void main() {
	int tileIndex = int((1 - outVertLighting) * unifNumTiles);
	vec2 tileUV = outVertUV;
	// tileUV.x = mod(tileUV.x * 2, 1);
	// tileUV.y = mod(tileUV.y * 2, 1);
	tileUV.x = mod(tileUV.x, 1);
	tileUV.x = tileUV.x * (1.0 / 6) + tileIndex * (1.0 / 6);
  outFragColor = texture(unifTilesTexture, tileUV);
}
