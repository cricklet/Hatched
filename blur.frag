#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;

uniform sampler2D unifBuffer;

float BLUR_RADIUS = 0.0005;

uniform sampler2D unifDepths;
uniform bool unifBlurDepthCheck;

void main () {
  vec2 origin = outVertBufferCoord;
  
  float depth;
  if (unifBlurDepthCheck) {
    depth = texture(unifDepths, origin).r;
  }

  vec4 result = vec4(0,0,0,1); 
  int samples = 0;
  for (int x = -4; x <= 4; x ++) {
    for (int y = -4; y <= 4; y ++) {
      vec2 sample = origin + vec2(x, y) * BLUR_RADIUS;
      sample = clamp(sample, 0,1);

      if (unifBlurDepthCheck) {
	float sampleDepth = texture(unifDepths, sample).r;
   	if (abs(depth - sampleDepth) > 0.001) continue;
      }
      
      result += texture(unifBuffer, sample);
      samples += 1;
    }
  }

  outFragColor = result / samples;
  //outFragColor = texture(unifBuffer, outVertBufferCoord);
}
