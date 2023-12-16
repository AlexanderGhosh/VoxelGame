#version 440 core

layout(location = 0) out vec4 accum;
layout(location = 1) out float reveal;

uniform sampler2D ao;
uniform vec3 lightPos;
uniform vec3 viewDir;


struct Material{
    vec4 albedo1;
    vec4 albedo2;
};
struct Light {
    vec3 Position;
    vec3 Color;
};

layout (std140, binding = 1) uniform Matrices
{
    Material[6] materials;
};

// out vec4 color;

flat in uint matIndex;
flat in vec2 rndSeed;
in vec3 fragPos;
in vec3 normal;

Light createLight();
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void main()
{
    Light light = createLight();
    float rnd = rand(rndSeed);
    Material mat = materials[matIndex];
    vec4 albedo = mix(mat.albedo1, mat.albedo2, rnd);

    if(albedo.a == 1) discard;

    // AO
    float occluded = texture(ao, gl_FragCoord.xy).r;
    occluded = 1.0;
    
    // blinn-phong (in model-space)
    vec3 lightDir = normalize(light.Position - fragPos);
    vec3 ambient = vec3(0.3 * albedo * occluded); // here we add occlusion factor
    // diffuse
    vec3 diffuse = 0.5 * max(dot(normal, lightDir), 0.0) * albedo.xyz * light.Color;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
    spec = max(spec, 0.5);
    vec3 specular = light.Color * spec;
    
    vec3 lighting = ambient;
    lighting += diffuse + specular;
    vec4 colour = vec4(lighting, albedo.a);

    
	// weight function
	float weight = clamp(pow(min(1.0, colour.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
	
	// store pixel color accumulation
	accum = vec4(colour.rgb, weight);

	// store pixel revealage threshold
	reveal = colour.a;
}


Light createLight() {
    Light light;
    light.Position = lightPos;
    light.Color = vec3(0.3);
    return light;
}
