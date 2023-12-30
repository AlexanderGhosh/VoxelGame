#version 460 core

layout(location = 0) out vec4 fragColour;

uniform vec4 colour;
uniform sampler2D text;

in vec2 texCoords;


void main()
{    
    fragColour = vec4(colour.rgb, colour.a * texture(text, texCoords).r);
}  