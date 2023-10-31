#version 440 core

layout (location = 0) out vec4 frag;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

// out vec4 color;

flat in uint blockColourIndex;
in vec3 Normal;
in vec3 FragPos;
in vec3 TexCoords;
in vec4 FragPosLightSpace;

Material material;

uniform samplerCube cubeMap; // samplerCube
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

const float light_constant = 0.4f;
const vec3 lightCol = vec3(0.3);

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

float shadowBias = 0.005;

float ShadowCalculation(vec4 fragPosLightSpace_);

void main()
{    
    vec4 color = vec4(0);
    vec4 colourA = texture(cubeMap, TexCoords);
    colourA = blockColours[blockColourIndex - 1u];
    material.ambient = vec3(0.25);
    material.diffuse = vec3(1);
    material.specular = vec3(0);
    material.shininess = 10;

    vec3 colour = colourA.rgb;
    // ambient
    vec3 ambient = material.ambient * colour; // 0.15

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * material.diffuse * lightCol;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * lightCol;    


    shadowBias = max(0.05 * (1.0 - dot(norm, lightDir)), shadowBias);
    float shadow = ShadowCalculation(FragPosLightSpace);   
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * colour;   
    color = vec4(lighting, colourA.a);
    // color = colourA;
    color.xyz = (Normal + 1) * 0.5;

    frag = color;
}


float ShadowCalculation(vec4 fragPosLightSpace_)
{
     // perform perspective divide
    vec3 projCoords = fragPosLightSpace_.xyz / fragPosLightSpace_.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / (textureSize(shadowMap, 0)*2);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
