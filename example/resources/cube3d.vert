#version 330 core

uniform mat4 mvp;

layout(location = 0) in vec3 posIn;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec2 uvIn;

out vec3 normal;
out vec2 uv;

void main() {
	gl_Position = mvp * vec4(posIn, 1.0);
	normal = normalIn;
	uv = uvIn;
}
