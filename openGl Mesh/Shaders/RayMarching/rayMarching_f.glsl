#version 440 core
#define MAX_STEPS 128
#define MIN_DIST 0.1
#define MAX_DIST 1000

layout(location = 0) out vec4 frag;
in vec2 texCoords;

layout(std430, binding = 1) buffer geometry {
    uint blockData[];
};

uniform vec3 viewPos;

struct HitInfo {
    bool hit;
    vec3 hitPos;
    vec3 normal;
};

float closestDistance(vec3 p);
HitInfo castRay();
vec3 getPos(uint index);
vec3 getNormal(vec3 pos);

void main() {
    vec3 color = vec3(0.3);
    HitInfo info = castRay();
    if(info.hit) {
        color = vec3(1, 0, 0);
        color = info.normal * .5 + .5;
    }
    frag = vec4(color, 1);
}

HitInfo castRay() {
    HitInfo res;
    res.hit = false;
    vec3 rPos = viewPos;
    vec3 rDir = normalize(vec3(texCoords * 2 - 1, -1));
    float t = 0;


    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 rEnd = rPos + rDir * t;
        float d = closestDistance(rEnd);
        if (d < MIN_DIST) {
            res.hit = true;
            res.hitPos = rEnd;
            res.normal = getNormal(rEnd);
            return res;
        }
        else if (d > MAX_DIST) {
            return res;
        }

        t += d;
    }
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float closestDistance(vec3 p) {
    float min_ = 100000;
    for(int i = 0; i < blockData.length(); i++) {
        vec3 c = getPos(i);
        c.z *= -1;
        float d = length(p - c) - 1.0;
        min_ = min(min_, d);
    }
    return min_;
}

vec3 getNormal(vec3 pos) {
    vec2 step_ = vec2(0.001, 0);
    
    // casts rays surrounding the hit pos to calc the gradiant
    float gradient_x = closestDistance(pos + step_.xyy) - closestDistance(pos - step_.xyy);
    float gradient_y = closestDistance(pos + step_.yxy) - closestDistance(pos - step_.yxy);
    float gradient_z = closestDistance(pos + step_.yyx) - closestDistance(pos - step_.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

    return normalize(normal);
}

vec3 getPos(uint index) {
    uint data = blockData[index];
    
    uint z = data & 0x0000000f;
    uint x = (data & 0x000000f0) >> 4;
    uint y = (data & 0x0000ff00) >> 8;

    return vec3(x, y, z);
}