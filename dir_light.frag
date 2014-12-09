#version 150

in vec3 outVertNorm;
in vec2 outVertUV;

out vec4 outFragColor;

uniform vec3 inColor;
uniform sampler2D inTex;

const vec3 lightDir = vec3(-1, -1, -1);

void main() {
  float light = dot(-normalize(lightDir), normalize(outVertNorm));
  outFragColor = light * vec4(inColor, 1.0) * texture(inTex, outVertUV);
}
