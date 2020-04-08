#version 330 core
layout(location = 0) in vec4 posTex;

uniform vec2 scale;
uniform vec2 modelPos;

uniform mat4 model;

out vec2 TexCoords;

void main() {
	vec2 pos = posTex.xy * scale;
	pos += modelPos;
	gl_Position = model * vec4(posTex.xy, 0, 1);

	TexCoords = posTex.zw;
}