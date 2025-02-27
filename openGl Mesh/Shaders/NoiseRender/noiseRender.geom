#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};

in VS_OUT {
    uint cubeType;
    uint blockColourIndex;
    mat4 m;
} vs_out[];

flat out uint blockColourIndex;
out vec3 normal;
out vec3 worldPos;
out vec4 viewFragPos_;
flat out vec2 rndSeed;

uniform float voxelSize;
// const float size = 0.5;
// const float inv_size = 1.0 / size;

const vec3 vertices[8] = vec3[](
	vec3(-0.5,-0.5,-0.5), // 0
	vec3( 0.5,-0.5,-0.5), // 1
	vec3(-0.5,-0.5, 0.5), // 2
	vec3( 0.5,-0.5, 0.5), // 3

	vec3(-0.5, 0.5,-0.5), // 4
	vec3( 0.5, 0.5,-0.5), // 5
	vec3( 0.5, 0.5, 0.5), // 6
	vec3(-0.5, 0.5, 0.5)  // 7
);

const int indices[] = int[](
    3, 2, 6, 7, // +z
    5, 4, 1, 0, // -z
    0, 4, 2, 7, // -x
    1, 3, 5, 6, // +x
    6, 7, 5, 4, // +y
    0, 2, 1, 3  // -y
);

const vec3 normals[6] = vec3[] (
    vec3(0, 0, 1),
    vec3(0, 0, -1),
    vec3(-1, 0, 0),
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, -1, 0)
);

void main() {
    blockColourIndex = 1;
    if(blockColourIndex == 5u) {
        return; // discards water
    }
    for(uint h = 0; h < 5; h++) {
        for (uint i = 0; i < 6; i++) {  
            normal = normals[i];

            for (uint j = 0u; j < 4u; j++){
                int l = indices[i * 4u + j];
                vec3 v = vertices[l];

                rndSeed = gl_in[0].gl_Position.xy + gl_in[0].gl_Position.zx;
                worldPos = (gl_in[0].gl_Position.xyz + v) * voxelSize;
                worldPos.y -= h;
                viewFragPos_ = view * vec4(worldPos, 1);

                gl_Position = projection * viewFragPos_;
                EmitVertex();
            }
        }
    }
    EndPrimitive();
}