#version 150

in vec3 outVertNorm;
in vec2 outVertUV;

out vec4 outFragColor;

uniform vec3 unifColor;
uniform sampler2D unifTexture;
uniform bool unifUseTexture;

const vec3 lightDir = vec3(-1, -1, -1);

void main() {
  float light = 0.5 + 0.5 * dot(-normalize(lightDir), normalize(outVertNorm));
  light = min(1, max(0, light));
  
  if (unifUseTexture) {
  	outFragColor = light * texture(unifTexture, outVertUV); 
  } else {
  	outFragColor = light * vec4(unifColor, 1.0);
  }
}
