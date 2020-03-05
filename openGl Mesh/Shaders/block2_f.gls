#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 viewPos;

vec3 lightCol = vec3(1);
vec3 lightPos = vec3( 50, 50, 50);

void main()
{    
    vec3 result = vec3(0);
    float amb_k = 0.75f;
    vec3 ambient = amb_k * vec3(texture(skybox, TexCoords));

    vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(skybox, TexCoords));

    // specular
    float specularStrength = 0.25;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(texture(skybox, TexCoords));

    result = (ambient + diffuse + specular) * lightCol;
    result == vec3(0);
    color = vec4(result, 1.0f);
}