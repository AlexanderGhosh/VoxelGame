#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

in VS_OUT {
    int cubeType;
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
    /*int[6] toInclude = int[](0, 0, 0, 0, 0, 0);
    int toIncludeSize = 0;
    int type = 0;
    for (int i = 0; i < inclusions.length();) {
        int count = inclusions[i];
        for (int j = 0; j < count && type == vs_out[0].cubeType; j++){
            toInclude[j] = inclusions[i + j + 1];
            toIncludeSize++;
        }
        type++;
        i += count;
    }*/

    /*for (int i = 0; i < toIncludeSize; i++) {
        for (int j = 0; j < 4; j++){ 
                int k = toInclude[i];
                int l = indices2[k * 4 + j];
                vec3 v = vertices[l];

                FragPos = vec3(vs_out[0].m * vec4(v, 1));
                FragPosLightSpace = vec3(lightSpaceMatrix * vec4(FragPos, 1));
                gl_Position = vs_out[0].vp * vec4(FragPos, 1);

                TexCoords = v * inv_size;
                TexCoords.y *= -1;

                Normal = normals[i];
                EmitVertex();
        }
        EndPrimitive();
    }*/

    for (int j = 0; j < 4; j++){ 
        int l = indices2[vs_out[0].cubeType * 4 + j];
        vec3 v = vertices[l];

        FragPos = vec3(vs_out[0].m * vec4(v, 1));
        FragPosLightSpace = vec3(lightSpaceMatrix * vec4(FragPos, 1));
        gl_Position = vs_out[0].vp * vec4(FragPos, 1);

        TexCoords = v * inv_size;
        TexCoords.y *= -1;

        Normal = normals[vs_out[0].cubeType];
        EmitVertex();
    }
    EndPrimitive();


    /*for (int i = 0; i < 12; i++) {
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
    }*/
}