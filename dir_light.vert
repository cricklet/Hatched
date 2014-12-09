#version 150

in vec3 inVertPosition;
in vec3 inVertNorm;
in vec2 inVertUV;
in vec3 inVertColor;

out vec3 outVertNorm;
out vec2 outVertUV;
out vec3 outVertColor;

uniform mat4 inVertModelTrans;
uniform mat4 inVertViewTrans;
uniform mat4 inVertProjTrans;

void main () {
  gl_Position = inVertProjTrans * inVertViewTrans * inVertModelTrans * vec4(inVertPosition, 1.0);
  outVertNorm = vec3(transpose(inverse(inVertModelTrans)) * vec4(inVertNorm, 0));
  outVertColor = inVertColor;
}
