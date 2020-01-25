#version 330 core

layout(location = 0) in vec3 posIn;
layout(location = 1) in vec3 colorIn;

out vec4 pos;
out vec3 color;

void main() {
  pos = vec4(posIn, 0.0);
  color = colorIn;
}
