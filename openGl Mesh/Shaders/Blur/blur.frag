#version 440 core

layout(location = 0) out float frag;

uniform sampler2D img;

in vec2 texCoords;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(img, 0));
    vec3 result = vec3(0);
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(img, texCoords + offset).rgb;
        }
    }
    result = result / (4.0 * 4.0);
    frag = result.r;
}