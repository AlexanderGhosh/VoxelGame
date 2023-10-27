#version 330 core

layout(location = 0) in vec4 worldPos_;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
    int cubeType;
    mat4 vp;
    mat4 m;
} vs_out;

void main() {
    vec3 offset = vec3(0, 30, 0);
    
    vs_out.cubeType = int(worldPos_.w);
    mat4 m = mat4(1);
    m[3][0] = worldPos_.x - 1;
    m[3][1] = worldPos_.y - 1;
    m[3][2] = worldPos_.z - 1;
    vs_out.m = m;

    vs_out.vp = projection * view;
}