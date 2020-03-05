#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

uniform vec3 scale;
out vec2 TexCoords;

void main() {
	vec3 pos = position;
    pos *= scale;
	gl_Position = vec4(pos, 1);

	TexCoords = texCoords;
}