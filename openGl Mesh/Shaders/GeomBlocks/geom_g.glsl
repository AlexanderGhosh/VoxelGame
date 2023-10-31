#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    uint cubeType;
    uint blockColourIndex;
    mat4 vp;
    mat4 m;
} vs_out[];
uniform mat4 lightSpaceMatrix;

flat out uint blockColourIndex;
out vec3 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;
const float size = 0.5;
const float inv_size = 1.0 / size;

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
int indices2[] = int[](
    3, 2, 6, 7, // +z
    5, 4, 1, 0, // -z
    0, 4, 2, 7, // -x
    1, 3, 5, 6, // +x
    6, 7, 5, 4, // +y
    0, 2, 1, 3  // -y
);

vec3 normals[6] = vec3[] (
    vec3(0, 0, 1),
    vec3(0, 0, -1),


    vec3(-1, 0, 0),
    vec3(1, 0, 0),


    vec3(0, 1, 0),
    vec3(0, -1, 0)

);
// the cude type is used as index the values are the indices of the faces to ignore

// 1st number is how many to include then followed by what to include
int[] inclusions = int[] (
    1, 0,
    1, 3, 
    1, 7,
    1, 2,
    1, 1,
    1, 6
);


void main() {
    blockColourIndex = vs_out[0].blockColourIndex;
    if(blockColourIndex == 5u){
        return; // will discard water
    }
    Normal = normals[vs_out[0].cubeType];
    for (uint j = 0u; j < 4u; j++){ 
        int l = indices2[vs_out[0].cubeType * 4u + j];
        vec3 v = vertices[l];

        FragPos = vec3(vs_out[0].m * vec4(v, 1));
        FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1);
        gl_Position = vs_out[0].vp * vec4(FragPos, 1);

        TexCoords = v * inv_size;
        TexCoords.y *= -1;

        EmitVertex();
    }
    EndPrimitive();
}