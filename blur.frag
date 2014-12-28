#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;
uniform sampler2D unifBuffer;

const float BLUR_RADIUS = 0.002;

void main () {
  vec4 result = vec4(0,0,0,1);
  int samples = 0;
  for (int x = -4; x <= 4; x ++) {
    for (int y = -4; y <= 4; y ++) {
      vec2 coord = outVertBufferCoord + vec2(x, y) * BLUR_RADIUS;
      result += texture(unifBuffer, coord);
      samples += 1;
    }
  }

  outFragColor = result / samples;
  //outFragColor = texture(unifBuffer, outVertBufferCoord);
}
