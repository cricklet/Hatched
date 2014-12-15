#version 150

in vec2 outVertBufferCoord;
out vec4 outFragColor;
uniform sampler2D unifBuffer;

void main() {
	outFragColor = texture(unifBuffer, outVertBufferCoord);
}
