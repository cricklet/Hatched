#version 150

in vec3 inVertPosition;
in vec3 inVertNorm;
in vec2 inVertUV;

// we want to output the world-space normal and position
out vec3 outVertPos;
out vec3 outVertNorm;
out vec2 outVertUV;
out float outVertDepth;

uniform mat4 unifModelTrans;
uniform mat4 unifViewTrans;
uniform mat4 unifProjTrans;

void main () {
  vec4 viewPos = unifProjTrans * unifViewTrans * unifModelTrans * vec4(inVertPosition, 1.0);
  gl_Position = viewPos;

  mat4 trans = unifModelTrans;

  outVertNorm = vec3(transpose(inverse(trans)) * vec4(inVertNorm, 0));
  outVertPos = vec3(trans * vec4(inVertPosition, 1.0));
  outVertUV = inVertUV;
  outVertDepth = length(viewPos);
}
