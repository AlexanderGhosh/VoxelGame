#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec3 TexCoords;
in vec4 FragPosLightSpace;

uniform samplerCube cubeMap; // samplerCube
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

const float light_constant = 0.4f;
const vec3 lightCol = vec3(0.5);

float shadowBias = 0.005;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - shadowBias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{    
    if(texture(cubeMap, TexCoords).a < 0.1)
        discard;
    vec3 colour = texture(cubeMap, TexCoords).rgb;

    // ambient
    vec3 ambient = light_constant * colour; // 0.15
    ambient = 0.3 * colour; // 0.15

    // diffuse
    vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * lightCol;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightCol;    


    shadowBias = max(0.05 * (1.0 - dot(norm, lightDir)), shadowBias);
    float shadow = ShadowCalculation(FragPosLightSpace);   
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * colour;   
    color = vec4(lighting, 1.0f);
}

