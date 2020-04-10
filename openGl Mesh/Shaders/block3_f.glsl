#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec3 TexCoords;

uniform samplerCube cubeMap;
uniform vec3 viewPos;

const float light_constant = 0.4f;
vec3 lightCol = vec3(1);
vec3 lightPos = vec3(-20, 100, -1000);

void main()
{    
    if(texture(cubeMap, TexCoords).a < 0.1)
        discard;
    vec3 result = vec3(0);
    vec3 ambient = light_constant * texture(cubeMap, TexCoords).rgb;

    vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
    lightDir = normalize(-lightPos);  
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_constant * vec3(texture(cubeMap, TexCoords));

    // specular
    float specularStrength = 0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * texture(cubeMap, TexCoords).rgb;

    result = (ambient + diffuse + specular) * lightCol;
    color = vec4(result, 1.0f);
}