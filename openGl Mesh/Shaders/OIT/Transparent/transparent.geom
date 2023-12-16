#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};
uniform float voxelSize;

in VS_OUT {
    uint cubeType;
    uint matIndex;
    mat4 m;
} vs_out[];

flat out uint matIndex;
flat out vec2 rndSeed;
out vec4 viewPos;
out vec3 fragPos;
out vec3 normal;


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
    matIndex = vs_out[0].matIndex;

    for (uint i = 0; i < 6; i++) {
        uint slot = vs_out[0].cubeType & (1 << i);
        if (slot > 0) {
            normal = normals[i];

            for (uint j = 0u; j < 4u; j++){ 
                int l = indices[i * 4u + j];
                vec3 v = vertices[l];
                
                if(matIndex == 5u && v.y > 0){ // is water and is a top vertex
                    v.y *= 0.75;
                }
                
                rndSeed = gl_in[0].gl_Position.xy + gl_in[0].gl_Position.zx;
                vec4 fragPos4 = vs_out[0].m * vec4(v * voxelSize, 1);
                fragPos = fragPos4.xyz;
                viewPos = view * fragPos4;
                gl_Position = projection * viewPos;
                EmitVertex();
            }
            EndPrimitive();
        }
    }
}