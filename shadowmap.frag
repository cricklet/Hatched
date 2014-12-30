#version 150

in float outVertDepth;

out vec4 outFrag;

void main() {

  outFrag = vec4(mod(outVertDepth,1),0.1,0.7,1.0);
}
