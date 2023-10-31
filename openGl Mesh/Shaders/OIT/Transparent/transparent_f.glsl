#version 440 core

layout(location = 0) out vec4 accum;
layout(location = 1) out float reveal;

// out vec4 color;

flat in uint blockColourIndex;


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
// 1 .9 .7

void main()
{
    vec4 color = blockColours[blockColourIndex - 1u];
	// weight function
	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
	
	// store pixel color accumulation
	accum = vec4(color.rgb * color.a, color.a) * weight;
	// store pixel revealage threshold
	reveal = color.a;

    // accum = vec4(0, 0, 1, 1);
    // reveal = 0.5;
}