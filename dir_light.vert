#version 150

in vec3 inVertPosition;
in vec3 inVertNorm;

out vec3 outVertNorm;

uniform mat4 inVertModelTrans;
uniform mat4 inVertViewTrans;
uniform mat4 inVertProjTrans;

void main () {
  gl_Position = inVertProjTrans * inVertViewTrans * inVertModelTrans * vec4(inVertPosition, 1.0);
  outVertNorm = vec3(transpose(inverse(inVertModelTrans)) * vec4(inVertNorm, 0));
}
