#version 440 core

layout(location = 0) out vec4 frag;

uniform sampler2D albedoPos;
uniform sampler2D normalRnd;
uniform sampler2D worldPos;
uniform sampler2D ao;
uniform sampler2D shadowMap;
uniform float numBlocks;
uniform mat4 view_inv;
uniform mat4 lightMatrix;
uniform vec3 viewDir;

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
struct Light {
    vec3 Position;
    vec3 Color;
};

Light createLight();
float inShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main() {
    Light light = createLight();
    vec4 albedoPosSample = texture(albedoPos, texCoords);
    float colorIndexNormalised = albedoPosSample.w;
    uint colourIndex = uint(colorIndexNormalised * numBlocks);
    // normal and rnd number
    vec4 normalRndSample = texture(normalRnd, texCoords);
    // AO
    float occluded = texture(ao, texCoords).r;
    // fragment world pos
    vec4 fragPosWorld = texture(worldPos, texCoords);

    float rndValue = normalRndSample.w;
    vec3 fragPosView = albedoPosSample.xyz;
    vec3 normal = normalRndSample.xyz;
    vec3 albedo = (blockColours[colourIndex]).rgb;

    vec4 lightFragPos = lightMatrix * vec4(fragPosWorld.xyz, 1);
    
    // adds random variation
    albedo += rndValue * 0.075;
        // pow(occluded, 3.0)

    // retrieve data from gbuffer
    
    // blinn-phong (in view-space)
    occluded = 1; // not occluded
    vec3 lightDir = normalize(light.Position - fragPosWorld.xyz);
    vec3 ambient = vec3(0.3 * albedo * occluded); // here we add occlusion factor
    vec3 lighting  = ambient;
    // diffuse
    vec3 diffuse = 0.5 * max(dot(normal, lightDir), 0.0) * albedo * light.Color;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
    vec3 specular = light.Color * spec;
    
    lighting += diffuse + specular;

    frag = vec4(lighting, 1);
}

Light createLight() {
    Light light;
    light.Position = vec3(100, 100, 100);
    light.Color = vec3(0.3);
    return light;
}


float inShadow(vec4 fragPosLight, vec3 normal, vec3 lightDir)
{
    float bias = max(0.05 * (1.0 - dot(abs(normal), lightDir)), 0.005);  
    //bias = 0.005;
    bias = 0;
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;
    // float shadow = texture(shadowMap, projCoords.xy).r; 
    float shadow = 0;
    if(projCoords.z > 1.0)
        return 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += projCoords.z + bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}