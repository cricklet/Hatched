#version 150

in vec3 inVertPosition;

uniform mat4 unifModelTrans;
uniform mat4 unifViewTrans;
uniform mat4 unifProjTrans;

void main () {
  gl_Position = unifProjTrans * unifViewTrans * unifModelTrans * vec4(inVertPosition, 1.0);
}
