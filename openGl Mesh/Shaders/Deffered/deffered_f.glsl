#version 440 core

layout(location = 0) out vec4 frag;

const int kernelSize = 64; // ssao req

uniform sampler2D albedoPos;
uniform sampler2D normalRnd;
// uniform sampler2D ao;
uniform float numBlocks;

// SSAO Reqs
uniform sampler2D ssaoNoise;
uniform vec3 ssaoSamples[kernelSize];
uniform vec2 ssaoNoiseScale;
uniform float ssaoRadius;
uniform float ssaoBias;
uniform mat4 projection;
uniform mat4 view;


in vec2 texCoords;

const vec4[] blockColours = vec4[] (
    vec4(.5, 0, .5, 1),
    vec4(0, 0.4, 0.1, 1),
    vec4(.5, .25, .25, 1),
    vec4(.65, .65, .65, 1),
    vec4(0, 0.6, 1, 0.5),
    vec4(.5, .25, .25, 1),
    vec4(1, 0.9, 0.7, 1), // sand
    vec4(1, 0.9, 0.7, 1)
);

void main() {
    vec4 albedoPosSample = texture(albedoPos, texCoords);
    float colorIndexNormalised = albedoPosSample.w;
    uint colourIndex = uint(colorIndexNormalised * numBlocks);
    vec4 normalRndSample = texture(normalRnd, texCoords);

    float rndValue = normalRndSample.w;
    vec3 pos = albedoPosSample.xyz;
    vec3 normal = normalRndSample.xyz;
    vec3 albedo = (blockColours[colourIndex]).rgb;
    
    // adds random variation
    albedo += rndValue * 0.075;
    
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
        offset.xyz /= offset.w;               // perspective divide
        offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        float sampleDepth = texture(albedoPos, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, ssaoRadius / abs(pos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + ssaoBias ? 1.0 : 0.0) * rangeCheck;   
    }  

    occlusion = 1.0 - (occlusion / float(kernelSize));


    // float occlusion = texture(ao, texCoords).r;

    // only outputs the albedo
    frag = vec4(albedo * occlusion, 1);
}