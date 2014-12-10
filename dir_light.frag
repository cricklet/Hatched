#version 150

in vec3 outVertNorm;
in vec2 outVertUV;

out vec4 outFragColor;

uniform vec3 unifColor;
uniform sampler2D unifTexture;

const vec3 lightDir = vec3(-1, -1, -1);

void main() {
  float light = dot(-normalize(lightDir), normalize(outVertNorm));
  outFragColor = light * texture(unifTexture, outVertUV); // * vec4(unifColor, 1.0);
}
