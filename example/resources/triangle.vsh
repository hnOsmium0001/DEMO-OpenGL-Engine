#version 330 core

in vec3 posIn;
in vec3 colorIn;

out vec3 pos;
out vec3 color;

void main() {
  pos = posIn;
  color = colorIn;
}
