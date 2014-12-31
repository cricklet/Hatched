#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;

uniform sampler2D unifBuffer;
uniform int unifBufferSize;
uniform vec2 unifBlurDir;


float GAUSS[] = float []
  (0.010324141203611343,
   0.041866457066584456,
   0.11380482946605731,
   0.20736591934533882,
   0.2532773058368162,
   0.20736591934533882,
   0.11380482946605731,
   0.041866457066584456,
   0.010324141203611343);

void main () {
  vec3 result = vec3(0,0,0);

  float pixelSize = 1.0 / unifBufferSize;
  for (int i = 0; i < GAUSS.length(); i ++) {
    float weight = GAUSS[i];

    vec2 uv = outVertBufferCoord;
    uv += unifBlurDir * pixelSize;
    uv = clamp(uv, 0,1);

    result += texture(unifBuffer, uv).rgb;
  }

  outFragColor = vec4(result, 1);
}
