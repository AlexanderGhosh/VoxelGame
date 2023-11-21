#version 440 core

layout(location = 0) out vec4 accum;
layout(location = 1) out float reveal;

struct Material{
    vec4 albedo;
};

layout (std140, binding = 0) uniform Matrices
{
    Material[6] materials;
};

// out vec4 color;

flat in uint blockColourIndex;
flat in vec3 fragCoords;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void main()
{
    vec4 color = materials[blockColourIndex].albedo;

    float rnd = rand(fragCoords.xz * fragCoords.y);

    color.rgb += rnd * 0.075;
    
	// weight function
	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
	
	// store pixel color accumulation
	accum = vec4(color.rgb, weight);

	// store pixel revealage threshold
	reveal = color.a;
}