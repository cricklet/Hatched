#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform mat4 unifInvViewTrans;
uniform mat4 unifInvProjTrans;
uniform mat4 unifViewTrans;
uniform mat4 unifProjTrans;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;
uniform sampler2D unifDepths;

uniform vec3 unifLightDir;

uniform sampler2D unifRandom; // stores a texture of random values

const float RADIUS = 0.2;
const int SAMPLES = 16;

const vec3 SAMPLE_HEMI [SAMPLES] = vec3 [SAMPLES]
  (vec3(-0.043242, 0.002109, 0.045077),
   vec3(-0.093851, -0.024186, 0.078944),
   vec3(-0.160933, -0.009406, 0.095751),
   vec3(0.003174, -0.080455, 0.236679),
   vec3(-0.092329, 0.194302, 0.226668),
   vec3(-0.134078, -0.156855, 0.313121),
   vec3(-0.250477, 0.267702, 0.238753),
   vec3(0.241081, 0.378695, 0.220160),
   vec3(-0.410303, 0.315175, 0.220732),
   vec3(-0.137216, 0.506383, 0.339665),
   vec3(0.456100, -0.426172, 0.288109),
   vec3(0.172611, -0.402651, 0.608751),
   vec3(-0.152679, -0.745084, 0.285825),
   vec3(-0.461858, -0.344764, 0.658369),
   vec3(-0.353674, 0.442379, 0.747075),
   vec3(-0.554254, 0.467453, 0.688687));

mat3x3 getRotationMatrix(vec3 surfaceNormal, vec3 randomNormal) {
  vec3 tangent = normalize(randomNormal - surfaceNormal * dot(randomNormal, surfaceNormal));
  vec3 bitangent = cross(surfaceNormal, tangent);
  return mat3x3(tangent, bitangent, surfaceNormal);
}

vec3 posWorldToView(vec3 wPos) {
  return vec3(unifViewTrans * vec4(wPos, 1));
}

vec3 normWorldToView(vec3 wNorm) {
  return vec3(transpose(unifInvViewTrans) * vec4(wNorm,1));
}

vec2 posViewToScreen(vec3 vPos) {
  vec4 pPos = unifProjTrans * vec4(vPos,1); // projection space
  return vec2(0.5,0.5) + 0.5 * pPos.xy / pPos.w;
}

void main() {
  // screen space
  vec2 sCoord = outVertBufferCoord;

  // world space
  vec3 wPos = texture(unifPositions, sCoord).xyz;
  vec3 wNorm = normalize(texture(unifNormals, sCoord).xyz);

  // view space
  vec3 vPos = posWorldToView(wPos);
  vec3 vNorm = normWorldToView(wNorm);
  float vDepth = length(vPos);

  // the farther the sample, the smaller the sphere of influence
  float radius = RADIUS / vDepth;

  // reflect each sample on a random plane
  vec3 randomNormal = texture(unifRandom, sCoord * mat2x2(24,0,0,16)).xyz;
  randomNormal = normalize(randomNormal - vec3(0.5,0.5,0.5));
  mat3x3 randomRotation = getRotationMatrix(vNorm, randomNormal);

  float occlusion = 0.0f;

  vec3 avgRay = vec3(0,0,0);
  for (int i = 0; i < SAMPLES; i ++) {
    vec3 vRay = SAMPLE_HEMI[i];
    vRay = radius * randomRotation * vRay;

    avgRay += vRay;
    
    // where the ray ends up
    vec3 vPos0 = vPos + vRay;
    float vDepth0 = length(vPos0);

    // what's actually there
    vec2 sCoord1 = posViewToScreen(vPos0);
    if (sCoord1.x < 0 || sCoord1.x > 1 || sCoord1.y < 0 || sCoord1.y > 1) continue;
    vec3 wPos1 = texture(unifPositions, sCoord1).xyz;
    vec3 vPos1 = posWorldToView(wPos1);
    float vDepth1 = length(vPos1);

    if (abs(vDepth1 - vDepth) < 0.1) {
      if (vDepth1 <= vDepth0) {
	occlusion += 1.0;
      }
    }
  }

  outFragColor = vec4(normalize(avgRay), 1);
  occlusion = 1 - occlusion / SAMPLES;
  outFragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
