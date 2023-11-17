#version 440 core
#define MAX_STEPS 128
#define MIN_DIST 0.1
#define MAX_DIST 1000

layout(location = 0) out vec4 frag;
in vec2 texCoords;

uniform vec3 viewPos;

float closestDistance(vec3 p);
bool castRay();

void main() {
    vec3 color = vec3(0.3);
    if(castRay()) {
        color = vec3(1, 0, 0);
    }
    frag = vec4(color, 1);
}

float closestDistance(vec3 p) {
    float d = length(p - vec3(0, 0, -10)) - 1.0;
    return d;
}

bool castRay() {
    vec3 rPos = viewPos;
    vec3 rDir = normalize(vec3(texCoords * 2 - 1, -1));
    float t = 0;


    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 rEnd = rPos + rDir * t;
        float d = closestDistance(rEnd);
        if (d < MIN_DIST) {
            return true;
        }
        else if (d > MAX_DIST) {
            return false;
        }

        t += d;
    }
}