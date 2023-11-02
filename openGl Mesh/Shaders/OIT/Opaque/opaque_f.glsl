#version 440 core

layout(location = 0) out vec4 frag;

uniform sampler2D shadowMap;
uniform vec3 lightPos;

// out vec4 color;

flat in uint blockColourIndex;
flat in vec3 fragCoords;
in vec4 fragPosLight;

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


void main()
{       
    frag = blockColours[blockColourIndex - 1u];
    float rnd = rand(fragCoords.xz * fragCoords.y);

    vec3 color = frag.rgb + (rnd * 0.075);

    //vec3 normal = normalize(fs_in.Normal);
    //vec3 lightColor = vec3(0.3);
    //// ambient
    //vec3 ambient = 0.3 * lightColor;
    //// diffuse
    //vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    //float diff = max(dot(lightDir, normal), 0.0);
    //vec3 diffuse = diff * lightColor;
    //// specular
    //vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = 0.0;
    //vec3 halfwayDir = normalize(lightDir + viewDir);  
    //spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    //vec3 specular = spec * lightColor;    
    //// calculate shadow
    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    //
    //FragColor = vec4(lighting, 1.0);

    frag.rgb = color;
}

float inShadow() {
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = 0.5 + 0.5 * projCoords;
    float sample_ = texture(shadowMap, projCoords.xy).r;
    float shadow = sample_ > projCoords.z ? 1 : 0;
    return shadow;
}