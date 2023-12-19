#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

in VS_OUT {
    uint cubeType;
    mat4 model;
} vs_out[];

uniform mat4 lightMatrix;

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

void main() {
    for (uint i = 0; i < 6; i++) {
        uint slot = vs_out[0].cubeType & (1 << i);
        if (slot > 0) {
            for (uint j = 0u; j < 4u; j++){ 
                int l = indices[i * 4u + j];
                vec3 v = vertices[l];

                gl_Position = lightMatrix * vs_out[0].model * vec4(v, 1);

                EmitVertex();
            }
            EndPrimitive();
        }
    }
}