#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

in VS_OUT {
    uint cubeType;
    mat4 vp;
    mat4 m;
} vs_out[];
uniform mat4 lightSpaceMatrix;

out vec3 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 FragPosLightSpace;
const float size = 0.5;
const float inv_size = 1 / size;

vec3 vertices[8] = vec3[](
	vec3(-size,-size,-size), // 0
	vec3( size,-size,-size), // 1
	vec3(-size,-size, size), // 2
	vec3( size,-size, size), // 3

	vec3(-size, size,-size), // 4
	vec3( size, size,-size), // 5
	vec3( size, size, size), // 6
	vec3(-size, size, size)  // 7
);

int indices[] = int[](
    3, 2, 6, // +z
    2, 6, 7,

    6, 7, 5, // +y
    7, 5, 4,

    3, 1, 6, // +x
    1, 6, 5,

    5, 4, 1,
    4, 1, 0,

    2, 0, 3,
    0, 3, 1,

    7, 4, 2,
    4, 2, 0
    
);

vec3 normals[6] = vec3[] (
    vec3(0, 0, 1),

    vec3(0, 1, 0),

    vec3(1, 0, 0),

    vec3(0, 0, -1),

    vec3(0, -1, 0),

    vec3(-1, 0, 0)
);
// the cude type is used as index the values are the indices of the faces to ignore



void main() {
/*
for (int i = 0; i < indices.length(); i += 4) {
    for (int j = 0; j < 4; j++){ 
            int k = indices[i * 3 + j];
            vec3 v = vertices[k];

            FragPos = vec3(vs_out[0].m * vec4(v, 1));
            FragPosLightSpace = vec3(lightSpaceMatrix * vec4(FragPos, 1));
            gl_Position = vs_out[0].vp * vec4(FragPos, 1);

            TexCoords = v * inv_size;
            TexCoords.y *= -1;

            Normal = normals[i / 2];
            EmitVertex();
    }
}
*/


    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 3; j++) {
            int k = indices[i * 3 + j];
            vec3 v = vertices[k];

            FragPos = vec3(vs_out[0].m * vec4(v, 1));
            FragPosLightSpace = vec3(lightSpaceMatrix * vec4(FragPos, 1));
            gl_Position = vs_out[0].vp * vec4(FragPos, 1);

            TexCoords = v * inv_size;
            TexCoords.y *= -1;

            Normal = normals[i / 2];
            EmitVertex();
        }
        EndPrimitive();
    }
}