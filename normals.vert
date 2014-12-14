#version 150

in vec3 inVertPosition;
in vec3 inVertNorm;

out vec3 outVertNorm;

uniform mat4 unifModelTrans;
uniform mat4 unifViewTrans;
uniform mat4 unifProjTrans;

void main () {
  gl_Position = unifProjTrans * unifViewTrans * unifModelTrans * vec4(inVertPosition, 1.0);
  mat4 normTrans = transpose(inverse(unifViewTrans * unifModelTrans));
  outVertNorm = normalize(vec3(normTrans * vec4(inVertNorm, 0)));
}
