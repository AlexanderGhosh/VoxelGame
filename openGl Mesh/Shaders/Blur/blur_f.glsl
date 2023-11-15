#version 440 core

layout(location = 1) out float frag;

uniform sampler2D img;

in vec2 texCoords;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(img, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(img, texCoords + offset).r;
        }
    }
    frag = (result / (4.0 * 4.0)).r;
}