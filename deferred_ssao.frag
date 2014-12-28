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

const float RADIUS = 0.1;
const int SAMPLES = 16;

const vec3 SAMPLE_HEMI [SAMPLES] = vec3 [SAMPLES]
  (vec3(-0.039423, 0.043469, 0.021507),
   vec3(0.012715, -0.107895, 0.061822),
   vec3(-0.055582, 0.121087, 0.131928),
   vec3(-0.138698, -0.141453, 0.152492),
   vec3(0.226842, -0.100861, 0.189806),
   vec3(0.290029, -0.139790, 0.192267),
   vec3(0.383359, 0.174784, 0.117868),
   vec3(0.155708, -0.391414, 0.269352),
   vec3(0.385975, 0.214144, 0.348672),
   vec3(0.395854, 0.154401, 0.458350),
   vec3(0.064392, -0.416466, 0.543200),
   vec3(0.272320, 0.536439, 0.447856),
   vec3(-0.030392, 0.289784, 0.758458),
   vec3(0.092127, 0.134697, 0.859648),
   vec3(0.640049, -0.115627, 0.675184),
   vec3(0.240898, -0.844194, 0.478857));

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

    if (abs(vDepth1 - vDepth) < 0.02) {
      if (vDepth1 <= vDepth0) {
	occlusion += 1.0;
      }
    }
  }

  outFragColor = vec4(normalize(avgRay), 1);
  occlusion = 1 - occlusion / SAMPLES;
  outFragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
