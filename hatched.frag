#version 150

in vec2 outVertUV;
in float outVertLighting;

out vec4 outFragColor;

uniform vec3 unifColor;
uniform sampler2D unifTilesTexture;
uniform int unifNumTiles;

void main() {
	float tileWeights[12]; // always must be greater than the number of tiles
	for (int i = 0; i < 12; i ++) {
		tileWeights[i] = 0;
	}
	
	// setting the corresponding tile weight
	float exact = (1 - outVertLighting) * unifNumTiles;
	int index = int(exact);
	float weight = exact - index;
	//weight = pow(weight, 4);
	
	tileWeights[index] = 1 - weight;
	tileWeights[index + 1] = weight;
	
	outFragColor = vec4(0,0,0,1);
	for (int i = 0; i < unifNumTiles; i ++) {
		vec2 tileUV = outVertUV;
		tileUV.x = mod(tileUV.x, 1);
		tileUV.x = tileUV.x * (1.0 / 6) + i * (1.0 / 6);
		outFragColor += tileWeights[i] * texture(unifTilesTexture, tileUV);
	}
}
