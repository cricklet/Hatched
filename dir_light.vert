#version 150

in vec3 inVertPosition;
in vec3 inVertNorm;
in vec2 inVertUV;

out vec3 outVertNorm;
out vec2 outVertUV;

uniform mat4 unifModelTrans;
uniform mat4 unifViewTrans;
uniform mat4 unifProjTrans;

void main () {
  gl_Position = unifProjTrans * unifViewTrans * unifModelTrans * vec4(inVertPosition, 1.0);
  outVertNorm = vec3(transpose(inverse(unifModelTrans)) * vec4(inVertNorm, 0));
  outVertUV = inVertUV;
}
