#version 440 core

layout(location = 0) out vec4 frag;

// out vec4 color;

flat in uint blockColourIndex;


const vec4[] blockColours = vec4[] (
    vec4(.5, 0, .5, 1),
    vec4(0, 1, 0, 1),
    vec4(.5, .25, .25, 1),
    vec4(.65, .65, .65, 1),
    vec4(0, 0, 1, 0.5),
    vec4(.5, .25, .25, 1),
    vec4(.25, 1, .25, .75),
    vec4(1, 1, 0, 1)
);


void main()
{
    frag = blockColours[blockColourIndex - 1u];
}