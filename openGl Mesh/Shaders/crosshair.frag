#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture0;

void main()
{    
    vec4 tex = texture(texture0, TexCoords);
    FragColor = tex;
}