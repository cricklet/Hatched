#version 150

in vec2 outVertUV;
in float outVertLighting;

out vec4 outFragColor;

uniform vec3 unifColor;
uniform sampler2D unifTilesTexture;
uniform int unifNumTiles;

void main() {
	// int tileIndex = min((lighting * unifNumTiles) % unifNumTiles, unifNumTiles - 1);
	vec2 tileUV = outVertUV;
	tileUV.x = mod(tileUV.x, 1);
	//tileUV.x = tileUV.x * (1.0 / 6);
  //outFragColor = texture(unifTilesTexture, tileUV);
  if (tileUV.x < 0) {
  	outFragColor = vec4(0,1,0,1);
  } else if (tileUV.x > 1) { // this happens wtf
		outFragColor = vec4(0,0,tileUV.x - 1,1);  
  } else {
    outFragColor = vec4(tileUV.x, 0, 0, 1);
  }
}
