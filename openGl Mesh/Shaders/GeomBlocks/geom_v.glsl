#version 330 core

layout(location = 0) in vec3 worldPos_;
layout(location = 1) in uint cubeType_;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    uint cubeType;
    mat4 mvp;
} vs_out;

void main() {
    vec3 offset = vec3(0, 30, 0);
    
    vs_out.cubeType = cubeType_;

    mat4 m = mat4(1);
    m[3][0] = worldPos_.x - 1;
    m[3][1] = worldPos_.y - 1;
    m[3][2] = worldPos_.z - 1;

    vs_out.mvp = projection * view * m;
}