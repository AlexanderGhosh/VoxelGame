#version 440 core

layout(location = 0) out vec4 frag;

uniform sampler2D albedoPos;
uniform sampler2D normalRnd;
uniform sampler2D ao;
uniform float numBlocks;

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
    float occluded = texture(ao, texCoords).r;

    float rndValue = normalRndSample.w;
    vec3 pos = albedoPosSample.xyz;
    vec3 normal = normalRndSample.xyz;
    vec3 albedo = (blockColours[colourIndex]).rgb;
    
    // adds random variation
    albedo += rndValue * 0.075;
    
    frag = vec4(albedo * occluded, 1);
}