#version 150

in vec2 outVertBufferCoord;

out vec4 outFragColor;

uniform mat4 unifModelTrans;
uniform mat4 unifViewTrans;
uniform mat4 unifProjTrans;

uniform sampler2D unifPositions;
uniform sampler2D unifNormals;
uniform sampler2D unifUVs;
uniform sampler2D unifDepths;

uniform vec3 unifLightDir;

uniform sampler2D unifRandom; // stores a texture of random values

const float RADIUS = 0.02;
const int SAMPLES = 16;
const vec3 SAMPLE_SPHERE [SAMPLES] = vec3 [SAMPLES]
  (vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
   vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
   vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
   vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
   vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
   vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
   vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
   vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271));


void main() {
  vec2 coord = outVertBufferCoord;

  vec3 vPosition = texture(unifPositions, coord).xyz;
  vec3 vNormal = normalize(texture(unifNormals, coord).xyz);
  float vDepth = length(vPosition);
  float cDepth = texture(unifDepths, coord).x;

  vec2 cPosition = vec2(unifProjTrans * vec4(vPosition, 1)) / vDepth;
  cPosition = (cPosition + vec2(1,1)) * 0.5;

  outFragColor = vec4(cPosition, 0, 1);
  //outFragColor = vec4(coord,0,1);

  float radius = RADIUS / vDepth;

  // reflect each sample on a random plane
  vec3 randomNormal = texture(unifRandom, coord * mat2x2(24,0,0,16)).xyz;
  randomNormal = normalize(randomNormal - vec3(0.5,0.5,0.5));

  vec3 avgRay = vec3(0,0,0);

  float occlusion = 0.0f;
  for (int i = 0; i < SAMPLES; i ++) {
    vec3 vRay = SAMPLE_SPHERE[i];
    vRay = reflect(vRay, randomNormal);
    vRay = radius * vRay;
    vRay = sign(dot(vRay, vNormal)) * vRay;

    vec3 vExpectedPosition = vPosition + vRay;
    float vExpectedDepth = length(vExpectedPosition);
    vec3 cExpectedPosition = vec3(unifProjTrans * vec4(vExpectedPosition, 1));

    vec2 vActualCoord = clamp(cExpectedPosition.xy, vec2(0,0), vec2(1,1));
    vec3 vActualPosition = texture(unifPositions, vActualCoord).xyz;
    float vActualDepth = length(vActualPosition);
    avgRay += vRay;

    if (vActualDepth <= vExpectedDepth) {
      occlusion += 1.0;
    }
  }

  //outFragColor = vec4(mod(vDepth, 1), 0, 0, 1.0);
  //outFragColor = vec4(vNormal, 1.0);
  //outFragColor = vec4(normalize(avgRay), 1.0);
  //occlusion = 1 - occlusion / SAMPLES;
  //outFragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
