#version 330 core

uniform float coefR;
uniform float coefG;
uniform float coefB;

in vec4 pos;
in vec3 color;

out vec4 fragColor;

void main() {
	fragColor = vec4(color.r * coefR, color.g * coefG, color.b * coefB, 1.0);
}
