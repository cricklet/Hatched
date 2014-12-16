#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;
uniform sampler2D unifNormals;
uniform sampler2D unifDepths;
uniform sampler2D unifRandom; // stores a texture of random values

const float RADIUS = 0.01;

const int SAMPLES = 16;
const vec3 SAMPLE_SPHERE [SAMPLES] = vec3 [SAMPLES] (
  vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
  vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
  vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
  vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
  vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
  vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
  vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
  vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271));

vec3 getNorm(vec2 pos) {
  return normalize(2 * texture(unifNormals, pos).rgb - vec3(1,1,1));
}

float getDepth(vec2 pos) {
  return texture(unifDepths, pos).r;
}

void main() {
  vec2 coord = outVertBufferCoord;
  vec3 normal = getNorm(coord);
  float depth = getDepth(coord);
  vec3 position = vec3(coord, depth);

  // the farther the sample, the smaller the sphere of influence
  float radius = RADIUS / depth;

  // reflect each sample on a random plane
  vec3 randomNormal = texture(unifRandom, coord * mat2x2(12,0,0,8)).xyz;
  randomNormal = normalize(randomNormal - vec3(0.5,0.5,0.5));

  float occlusion = 0.0f;

  vec3 averageRay = vec3(0,0,0);
  for (int i = 0; i < SAMPLES; i ++) {
    vec3 hemiRay = SAMPLE_SPHERE[i];
    hemiRay = reflect(hemiRay, randomNormal);
    hemiRay = radius * hemiRay;
    hemiRay = sign(dot(hemiRay, normal)) * hemiRay;

    vec3 flatRay = hemiRay - dot(hemiRay, normal) * normal;

    vec3 expectedPosition = position + flatRay;
    float expectedDepth = expectedPosition.z;

    vec2 sampleCoord = clamp(expectedPosition.xy, vec2(0,0), vec2(1,1));
    vec3 sampleNormal = getNorm(sampleCoord);
    float sampleDepth = getDepth(sampleCoord);

    float depthDifference = expectedDepth - sampleDepth; // positive, then occlusion
    //float normDifference = abs(1.0 - dot(sampleNormal, normal));
    
    if (abs(depthDifference) < 0.02) {
      occlusion += (depthDifference > 0 ? 1.0 : 0.0);
    }
  }

  occlusion = 1 - 1.5 * occlusion / SAMPLES;

  //outFragColor = vec4(normal, 1.0);
  outFragColor = vec4(occlusion, 0, 0, 1.0);
}
