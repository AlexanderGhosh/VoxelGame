#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

in VS_OUT {
    uint cubeType;
    mat4 mvp;
} vs_out[];

out vec3 TexCoords;
out vec3 Normal;
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

int elements[14] = int[](
	3, 2, 6, 7, 4, 2, 0,
	3, 1, 6, 5, 4, 1, 0
);

vec3 normals[12] = vec3[] (
    vec3(0, 0, 1),
    vec3(0, 0, 1),

    vec3(0, 1, 0),
    vec3(-1, 0, 0),

    vec3(-1, 0, 0),
    vec3(0, -1, 0),


    vec3(0, -1, 0),
    vec3(1, 0, 0),

    vec3(1, 0, 0),
    vec3(0, 1, 0),

    vec3(0, 0, -1),
    vec3(0, 0, -1) 
);

void build_face()
{
    for(int i = 0; i < 12; i++) {
        for (int j = 0; j < 3; j++) {
            int k = elements[i + j];
            vec3 v = vertices[k];
            gl_Position = vs_out[0].mvp * vec4(v, 1);
            TexCoords = v * inv_size;
            TexCoords.y *= -1;

            Normal = normals[i];
            EmitVertex();
        }
        EndPrimitive();
    }

    /*
    float size = 0.5;
    gl_Position = vs_out[0].mvp * vec4(-size, -size, 0.0, 1.0);    // 1:bottom-left
    
    EmitVertex();   

    gl_Position = vs_out[0].mvp * vec4( size, -size, 0.0, 1.0);    // 2:bottom-right
    
    EmitVertex();

    gl_Position = vs_out[0].mvp * vec4(-size,  size, 0.0, 1.0);    // 3:top-left
    
    EmitVertex();

    gl_Position = vs_out[0].mvp * vec4( size,  size, 0.0, 1.0);    // 4:top-right
    
    EmitVertex();
    
    EndPrimitive();*/
}

void main() {    
    build_face();
}