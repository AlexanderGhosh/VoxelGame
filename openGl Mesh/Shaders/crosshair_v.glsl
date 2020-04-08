#version 330 core
layout(location = 0) in vec4 posTex;

uniform mat4 model;

out vec2 TexCoords;

void main() {
	gl_Position = model * vec4(posTex.xy, 0, 1);

	TexCoords = posTex.zw;
}