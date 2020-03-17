#version 330 core

out vec4 fragCol;

uniform vec3 colour;

void main(){
    fragCol = vec4(colour, 1);
}