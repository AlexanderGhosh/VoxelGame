#version 460 core

layout (location = 0) out vec4 fragColour;

uniform float cornerRadius;
uniform vec2 position;
uniform vec2 dimentions;

in vec4 o_colour;


float roundedBoxSDF(vec2 center, vec2 size, float radius);

void main()
{
    float edgeSoftness = 0.5;

    vec2 half_dim = dimentions * 0.5;
    float alpha = o_colour.a;
    if(cornerRadius > 0){
        float d = roundedBoxSDF(gl_FragCoord.xy - position - half_dim, half_dim, cornerRadius);
        alpha = alpha - smoothstep(0.0f, edgeSoftness * 2.0, d);
    }
    fragColour = vec4(o_colour.rgb, alpha);
}

float roundedBoxSDF(vec2 center, vec2 size, float radius) {
    return length(max(abs(center) - size + radius, 0.0)) - radius;
}