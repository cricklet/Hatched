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

const float HEMI_MAX_DELTA = 2; // multiplied by radius
const float HEMI_RADIUS = 0.04;
const int HEMI_SAMPLES = 24;
const vec3 HEMI [] = vec3 []
  (vec3(0.015389, -0.009998, 0.025293),
   vec3(0.015600, -0.006110, 0.060213),
   vec3(0.063122, -0.057304, 0.039000),
   vec3(-0.059197, 0.082619, 0.072766),
   vec3(-0.021202, -0.072748, 0.136646),
   vec3(0.030301, -0.164969, 0.083805),
   vec3(-0.079214, 0.066942, 0.192602),
   vec3(0.164494, 0.015550, 0.187616),
   vec3(-0.130456, 0.117511, 0.219713),
   vec3(-0.111019, -0.191344, 0.220723),
   vec3(-0.273040, -0.136633, 0.157939),
   vec3(-0.154954, -0.092654, 0.328678),
   vec3(0.181107, -0.238093, 0.274865),
   vec3(0.231614, -0.357139, 0.101057),
   vec3(0.172083, 0.049933, 0.433152),
   vec3(0.303195, -0.164255, 0.362068),
   vec3(0.296229, -0.345218, 0.274408),
   vec3(-0.130982, 0.432870, 0.334475),
   vec3(0.425005, -0.239245, 0.338632),
   vec3(-0.228214, -0.298471, 0.499458),
   vec3(-0.097931, -0.302180, 0.574248),
   vec3(0.491444, -0.194473, 0.439681),
   vec3(0.655396, 0.232571, 0.181573),
   vec3(0.210937, 0.536386, 0.479892));

const float DISK_MAX_DELTA = 0.05;
const vec2 DISK_RADIUS = vec2(0.008, 0.006) * 0.5;
const int DISK_SAMPLES = 16;
const vec2 DISK[] = vec2[] // These are the Poisson Disk Samples
  (vec2( -0.94201624,  -0.39906216 ),
   vec2(  0.94558609,  -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2(  0.34495938,   0.29387760 ),
   vec2( -0.91588581,   0.45771432 ),
   vec2( -0.81544232,  -0.87912464 ),
   vec2( -0.38277543,   0.27676845 ),
   vec2(  0.97484398,   0.75648379 ),
   vec2(  0.44323325,  -0.97511554 ),
   vec2(  0.53742981,  -0.47373420 ),
   vec2( -0.26496911,  -0.41893023 ),
   vec2(  0.79197514,   0.19090188 ),
   vec2( -0.24188840,   0.99706507 ),
   vec2( -0.81409955,   0.91437590 ),
   vec2(  0.19984126,   0.78641367 ),
   vec2(  0.14383161,  -0.14100790 ));

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

  float disk_occlusion = 0.0f;
  {
    float random = texture(unifRandom, sCoord * mat2x2(24,0,0,16)).r * 2 * 3.14159;
    mat2x2 randomRotation = mat2x2
      (cos(random), -sin(random),
       sin(random), cos(random));

    outFragColor = vec4(0,0,0,1);

    vec3 avgRay = vec3(0,0,0);
    for (int i = 0; i < DISK_SAMPLES; i ++) {
      vec2 sRay = randomRotation * DISK[i] * DISK_RADIUS;
      vec2 sSampleCoord = clamp(sCoord + sRay, 0,1);
    
      vec3 wSamplePos = texture(unifPositions, sSampleCoord).xyz;
      vec3 vSamplePos = posWorldToView(wSamplePos);
      vec3 vSampleDir = normalize(vSamplePos - vPos);
      
      float vSampleDist = length(vSamplePos - vPos);

      if (vSampleDist > DISK_MAX_DELTA) continue;
      disk_occlusion += max(dot(vNorm, vSampleDir), 0);
    }
  }

  float hemi_occlusion = 0.0f;
  {
    // reflect each sample on a random plane
    vec3 randomNormal = texture(unifRandom, sCoord * mat2x2(24,0,0,16)).xyz;
    randomNormal = normalize(randomNormal - vec3(0.5,0.5,0.5));
    mat3x3 randomRotation = getRotationMatrix(vNorm, randomNormal);

    float radius = HEMI_RADIUS / max(vDepth, 1);

    for (int i = 0; i < HEMI_SAMPLES; i ++) {
      vec3 vRay = HEMI[i];
      vRay = radius * randomRotation * vRay;

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

      float vDelta = abs(vDepth1 - vDepth);
      if (vDelta < HEMI_MAX_DELTA * radius && vDepth1 <= vDepth0) {
	hemi_occlusion += 1.0;
      }
    }
  }

  float light = 1
    - 1 * disk_occlusion / DISK_SAMPLES
    - 0.5 * hemi_occlusion / HEMI_SAMPLES;
  outFragColor = vec4(light, light, light, 1.0);
}
