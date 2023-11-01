#version 440 core

layout(location = 0) out vec4 frag;

uniform sampler2D depthMap;

// out vec4 color;

flat in uint blockColourIndex;
flat in vec3 fragCoords;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

const vec4[] blockColours = vec4[] (
    vec4(.5, 0, .5, 1),
    vec4(0, 0.4, 0.1, 1),
    vec4(.5, .25, .25, 1),
    vec4(.65, .65, .65, 1),
    vec4(0, 0.6, 1, 0.5),
    vec4(.5, .25, .25, 1),
    vec4(.25, 1, .25, .75),
    vec4(1, 0.9, 0.7, 1)
);


void main()
{
    frag = blockColours[blockColourIndex - 1u];
    float rnd = rand(fragCoords.xz * fragCoords.y);

    frag.rgb = frag.rgb + (rnd * 0.075);
}