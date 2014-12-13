#version 150

in vec3 inVertPosition;
in vec3 inVertNorm;
in vec2 inVertUV;

out vec2 outVertUV;
out float outVertLighting;

uniform mat4 unifModelTrans;
uniform mat4 unifViewTrans;
uniform mat4 unifProjTrans;
uniform vec3 unifLightDir;

void main () {
  gl_Position = unifProjTrans * unifViewTrans * unifModelTrans * vec4(inVertPosition, 1.0);
  vec3 norm = vec3(transpose(inverse(unifModelTrans)) * vec4(inVertNorm, 0));
  outVertUV = inVertUV;
  outVertLighting = 0.5 + 0.5 * dot(-normalize(unifLightDir), normalize(norm));
}
