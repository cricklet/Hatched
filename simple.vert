#version 150

in vec3 inVertPosition;

uniform mat4 inVertModelTrans;
uniform mat4 inVertViewTrans;
uniform mat4 inVertProjTrans;

out vec2 outVertTexCoord;

void main () {
  gl_Position = inVertProjTrans * inVertViewTrans * inVertModelTrans * vec4(inVertPosition, 1.0);
}
