#version 440 core

layout(location = 0) out float ao;
layout (std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
};

const int kernelSize = 64; // ssao req

uniform sampler2D fragPosTex;
uniform sampler2D normalRnd;

// SSAO Reqs
uniform sampler2D ssaoNoise;
uniform vec3 ssaoSamples[kernelSize];
uniform vec2 ssaoNoiseScale;
uniform float ssaoRadius;
uniform float ssaoBias;
uniform float voxelSize;

in vec2 texCoords;

void main() {
    vec4 albedoPosSample = texture(fragPosTex, texCoords);
    vec4 normalRndSample = texture(normalRnd, texCoords);

    mat3 normalMatrix = transpose(inverse(mat3(view)));
    vec3 pos = (view * vec4(albedoPosSample.xyz, voxelSize)).xyz;
    vec3 normal = normalMatrix * normalRndSample.xyz;
    
    // ssao
    vec3 randomVec = texture(ssaoNoise, texCoords * ssaoNoiseScale).xyz; 
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);  


    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; i++)
    {
        // get sample position
        vec3 samplePos = TBN * ssaoSamples[i]; // from tangent to view-space
        samplePos = pos + samplePos * ssaoRadius;

        vec4 offset = vec4(samplePos, 1.0);
        offset      = projection * offset;    // from view to clip-space
        offset.xy /= offset.w;               // perspective divide
        offset.xy  = offset.xy * 0.5 + 0.5;   // transform to range 0.0 - 1.0
        vec4 sampleFragPos = view * vec4(texture(fragPosTex, offset.xy).xyz, 1.0);
        float sampleDepth = sampleFragPos.z;

        float rangeCheck = smoothstep(0.0, 1.0, ssaoRadius / abs(pos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + ssaoBias ? 1.0 : 0.0) * rangeCheck;
    }  

    occlusion = 1.0 - (occlusion / kernelSize);

    // only outputs the albedo
    ao = occlusion;
}