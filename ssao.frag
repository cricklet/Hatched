#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;
uniform sampler2D unifBuffer; // stores normals + depths
uniform sampler2D unifRandom; // stores a texture of random values

const float TOTAL_STRENGTH = 1.0;
const float BASE = 0.2;
  
const float AREA = 0.0075;
const float FALLOFF = 0.000001;

const float RADIUS = 0.001;

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

void main() {
  vec3 normal = texture(unifBuffer, outVertBufferCoord).rgb;
  float depth = texture(unifBuffer, outVertBufferCoord).a;

  // isometric screenspace coordinate system
  vec3 position = vec3(outVertBufferCoord, depth);

  // the farther the sample, the smaller the sphere of influence
  float radius = RADIUS / depth;

  // reflect each sample on a random plane
  vec3 random = texture(unifRandom, outVertBufferCoord * 4.0).rgb;

  // let's add up the amount of occlusion!
  float occlusion = 0.0;
  for (int i = 0; i < SAMPLES; i ++) {
    vec3 ray = radius * reflect(SAMPLE_SPHERE[i], random);
    //ray = sign(dot(ray, normal)) * ray;

    vec3 sample = position + ray;
    vec2 samplePosition = clamp(sample.xy, 0, 1);

    float actualDepth = texture(unifBuffer, samplePosition).a;

    // (sample > actual) = occluded
    float difference = depth - actualDepth;

    occlusion += step(FALLOFF, difference) * (1.0 - smoothstep(FALLOFF, AREA, difference));
  }

  float ao = 1.0 - TOTAL_STRENGTH * occlusion * (1.0 / SAMPLES);
  outFragColor = vec4(ao,0,0, 1);
}
