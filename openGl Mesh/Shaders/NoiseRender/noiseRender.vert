#version 440 core
#define CHUNK_SIZE 16

layout(location = 0) in float noise;

uniform float voxelSize;
uniform vec3 chunkPosition;

out VS_OUT {
    uint cubeType;
    uint blockColourIndex;
    mat4 m;
} vs_out;

float getHeight() {
    return float(noise);
}

void main() {
    float z = gl_VertexID / CHUNK_SIZE;
    float x = gl_VertexID % CHUNK_SIZE;
    float y = getHeight();

    vec3 worldPos = chunkPosition + vec3(x, y, z);
    worldPos -= vec3(0.5);

    gl_Position = vec4(worldPos, 1);
}