#version 150

in vec3 outVertNorm;
in vec2 outVertUV;
in vec3 outVertColor;

out vec4 outFragColor;

uniform sampler2D inTex;

const vec3 lightDir = vec3(-1, -1, -1);

void main() {
  float light = dot(-normalize(lightDir), normalize(outVertNorm));
  outFragColor = light * vec4(outVertColor, 1.0);// * texture(inTex, outVertUV);
}
