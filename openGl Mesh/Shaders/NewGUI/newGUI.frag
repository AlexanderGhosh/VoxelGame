#version 460 core

layout (location = 0) out vec4 fragColour;

uniform mat4 projection; // GUI projection not the camera one
uniform float cornerRadius;
uniform vec2 position;
uniform vec2 dimentions;
uniform vec4 bgColour;
uniform vec4 borderColour;
uniform float borderSize;


float roundedBoxSDF(vec2 center, vec2 size, float radius);

void main()
{
    float edgeSoftness = 0.0;
    vec4 colour = bgColour;

    mat4 inv_proj = inverse(projection);
    vec2 half_dim = dimentions * 0.5;
    float alpha = colour.a;
    float d = 0;
    if(cornerRadius > 0){
        d = roundedBoxSDF(gl_FragCoord.xy - position - half_dim, half_dim, cornerRadius);
        alpha = alpha - smoothstep(0.0, edgeSoftness * 2.0, d);
    }
    if (borderSize > 0) {
        float d1 = roundedBoxSDF(gl_FragCoord.xy - position - half_dim, half_dim - borderSize * 2.0, cornerRadius);
        if(d1 >= 0 && d <= 0) {
            colour = borderColour;
        }
    }
    fragColour = vec4(colour.rgb, alpha);
}

float roundedBoxSDF(vec2 center, vec2 size, float radius) {
    return length(max(abs(center) - size + radius, 0.0)) - radius;
} 