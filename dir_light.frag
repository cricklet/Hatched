#version 150

in vec2 outVertUV;
in float outVertLighting;

out vec4 outFragColor;

uniform vec3 unifColor;
uniform sampler2D unifTexture;
uniform bool unifUseTexture;

void main() {
  if (unifUseTexture) {
    outFragColor = outVertLighting * texture(unifTexture, outVertUV);
  } else {
    outFragColor = outVertLighting * vec4(unifColor, 1.0);
  }
}
