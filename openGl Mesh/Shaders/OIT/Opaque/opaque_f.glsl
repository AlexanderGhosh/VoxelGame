#version 440 core

layout(location = 0) out vec4 frag;

uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

// out vec4 color;

flat in uint blockColourIndex;
flat in vec3 fragCoords;
in vec4 fragPosLight;
in vec3 normal;
in vec3 fragPos;

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


float inShadow();
float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{       
    frag = blockColours[blockColourIndex - 1u];
    float rnd = rand(fragCoords.xz * fragCoords.y);

    vec3 color = frag.rgb + (rnd * 0.075);


    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shadow
    //float shadow = ShadowCalculation(fragPosLight);
    float shadow = inShadow();
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    // FragColor = vec4(lighting, 1.0);

    frag =  vec4(lighting, 1.0);
    //frag.rgb = color;
}

float inShadow() {
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;
    float sample_ = texture(shadowMap, projCoords.xy).r;
    float shadow = sample_ > projCoords.z ? 0 : 1;
    return shadow;
}

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
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}
