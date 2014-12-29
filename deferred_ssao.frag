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

const float RADIUS = 0.05;
const int SAMPLES = 24;

const vec3 SAMPLE_HEMI [SAMPLES] = vec3 [SAMPLES]
  (vec3(-0.015990, 0.019437, 0.018523),
   vec3(0.061055, -0.001838, 0.013236),
   vec3(-0.037433, -0.013196, 0.084934),
   vec3(-0.039510, 0.081507, 0.086143),
   vec3(-0.141557, -0.057427, 0.032828),
   vec3(-0.133578, 0.001640, 0.131570),
   vec3(0.128592, -0.124372, 0.125885),
   vec3(-0.136366, -0.001772, 0.209526),
   vec3(0.091656, -0.242861, 0.108257),
   vec3(-0.089110, -0.141121, 0.264198),
   vec3(0.193975, -0.186933, 0.213527),
   vec3(-0.101452, 0.033939, 0.359417),
   vec3(0.307315, -0.032423, 0.263715),
   vec3(-0.143698, -0.199237, 0.362024),
   vec3(-0.267594, 0.146025, 0.356086),
   vec3(0.362607, 0.244561, 0.242293),
   vec3(0.474775, 0.217291, 0.097981),
   vec3(-0.301226, 0.468055, 0.081204),
   vec3(0.336214, 0.364636, 0.326404),
   vec3(-0.477148, 0.258869, 0.309745),
   vec3(0.418468, 0.056249, 0.502379),
   vec3(-0.503985, 0.223432, 0.410772),
   vec3(-0.236030, 0.627676, 0.258678),
   vec3(0.573189, -0.449068, 0.179701));

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
  int samples = 0;

  outFragColor = vec4(0,0,0,1);

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
    if (sCoord1.x < 0 || sCoord1.y < 0 || sCoord1.x > 1 || sCoord1.y > 1) {
      outFragColor = vec4(1,0,0,1);
      continue;
    }

    vec3 wPos1 = texture(unifPositions, sCoord1).xyz;
    if (wPos1.x == 0 && wPos1.y == 0 && wPos1.z == 0) continue;

    vec3 vPos1 = posWorldToView(wPos1);
    float vDepth1 = length(vPos1);

    if (abs(vDepth1 - vDepth) < 2 * radius) {
      if (vDepth1 <= vDepth0) {
	occlusion += 1.0;
      }
    }
  }

  float light = 1 - occlusion / SAMPLES;
  outFragColor = vec4(light, light, light, 1.0);
}
