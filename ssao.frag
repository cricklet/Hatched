#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;
uniform sampler2D unifNormals;
uniform sampler2D unifDepths;
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

const vec3 SAMPLE_HEMI [SAMPLES] = vec3 [SAMPLES]
  (vec3(-0.023533, 0.022210, 0.053471),
   vec3(0.077950, -0.080397, 0.055545),
   vec3(-0.172249, 0.037271, 0.064012),
   vec3(-0.157773, 0.171449, 0.090625),
   vec3(-0.042653, 0.308388, 0.027090),
   vec3(-0.034433, -0.336498, 0.161890),
   vec3(0.332451, -0.256513, 0.122815),
   vec3(0.257295, 0.319564, 0.285793),
   vec3(0.511659, 0.141492, 0.185988),
   vec3(0.297776, -0.355047, 0.419400),
   vec3(-0.339902, 0.064817, 0.594072),
   vec3(0.465915, -0.202060, 0.551902),
   vec3(0.121715, -0.676518, 0.433204),
   vec3(-0.692766, -0.442972, 0.299126),
   vec3(0.728764, -0.095807, 0.581919),
   vec3(-0.721410, 0.294686, 0.626680));

float getDepth(vec2 pos) {
  return texture(unifDepths, pos).r;
}

vec3 getNorm(vec2 pos) {
  float depth = getDepth(pos);

  const vec2 offset1 = vec2(-0.0001,0.0001);
  const vec2 offset2 = vec2(0.0001,0.0001);
  
  float depth1 = getDepth(pos + offset1);
  float depth2 = getDepth(pos + offset2);
  
  vec3 p1 = vec3(offset1, depth1 - depth);
  vec3 p2 = vec3(offset2, depth2 - depth);
  
  vec3 normal = cross(p1, p2);
  normal.z = -normal.z;
  
  return normalize(normal);
  //return normalize(2 * texture(unifNormals, pos).rgb - vec3(1,1,1));
}

mat3x3 getRotationMatrix(vec3 surfaceNormal, vec3 randomNormal) {
  vec3 tangent = normalize(randomNormal - surfaceNormal * dot(randomNormal, surfaceNormal));
  vec3 bitangent = cross(surfaceNormal, tangent);
  return mat3x3(tangent, bitangent, surfaceNormal);
}

void main() {
  vec2 coord = outVertBufferCoord;
  vec3 normal = getNorm(coord); // should have positive z
  float depth = getDepth(coord); // should be positive
  vec3 position = vec3(coord, -depth);

  // the farther the sample, the smaller the sphere of influence
  float radius = RADIUS / depth;

  // reflect each sample on a random plane
  vec3 randomNormal = texture(unifRandom, coord * mat2x2(24,0,0,16)).xyz;
  randomNormal = normalize(randomNormal - vec3(0.5,0.5,0.5));

  float occlusion = 0.0f;

  mat3x3 randomRotation = getRotationMatrix(normal, randomNormal);

  vec3 avgRay = vec3(0,0,0);
  vec3 avgPos = vec3(0,0,0);

  for (int i = 0; i < SAMPLES; i ++) {
    vec3 hemiRay = SAMPLE_SPHERE[i];
    hemiRay = reflect(hemiRay, randomNormal);
    hemiRay = radius * hemiRay;
    hemiRay = sign(dot(hemiRay, normal)) * hemiRay;

    //vec3 hemiRay = SAMPLE_HEMI[i];
    //hemiRay = radius * randomRotation * hemiRay;

    vec3 expectedPosition = position + hemiRay;
    float expectedDepth = abs(expectedPosition.z);

    vec2 actualCoord = clamp(expectedPosition.xy, vec2(0,0), vec2(1,1));
    float actualDepth = getDepth(actualCoord);

    if (abs(actualDepth - depth) < 0.001) {
      if (actualDepth <= expectedDepth) {
	occlusion += 1.0;
      }
    }
  }

  occlusion = 1 - occlusion / SAMPLES;
  outFragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
