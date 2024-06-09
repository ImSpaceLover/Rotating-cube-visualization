#version 460
#define MAX_DIST 200.0
#define EPSILON 0.01
#define MAX_STEPS 150
#define PI 3.141592
#define M_SPEED 1.

uniform float u_time;
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform vec3 u_rotation;

in vec4 gl_FragCoord;
out vec4 fragColor;

mat2x2 rot(float phi) {
    return mat2(cos(phi), -sin(phi), sin(phi), cos(phi));
}

float sdBox(vec3 p, vec3 b ){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float smin(float a, float b, float k)
{
    k *= 1.0;
    float r = exp2(-a/k) + exp2(-b/k);
    return -k*log2(r);
}

float getdist(vec3 pos) {
    vec3 boxPos = pos - vec3(0, 0, 0);
    vec3 rotBox = u_rotation / 360 * PI;
    float plane = pos.y + 1;

    boxPos.xz = boxPos.xz - round(boxPos.xz/10)*10;
    boxPos.xz *= rot(rotBox.y);
    boxPos.xy *= rot(rotBox.z);
    boxPos.yz *= rot(rotBox.x);

    float box = sdBox(boxPos, vec3(1, 1, 1));

    float dist = smin(box, plane, 0.1);

    return dist;
}

vec3 getnormal(vec3 p) {
    float eps = 0.01;
    vec2 h = vec2(eps, 0.);
    float dist = getdist(p);
    return normalize(vec3(getdist(p + h.xyy) - dist,
                        getdist(p + h.yxy) - dist,
                        getdist(p + h.yyx) - dist));
}

float raymarch(vec3 ro, vec3 rd) {
    float dO = 0.;
    float dS = 0.;
    float dClosest = MAX_DIST;
    vec3 pos = ro;
    for (int i = 0; i < MAX_STEPS; i++) {
        dS = getdist(pos);
        dO += dS;
        pos = ro + rd*dO;
        if (dS < EPSILON || dO > MAX_DIST) break;
    }
    return min(dO, MAX_DIST);
}

float getlight(vec3 ro, vec3 lp) {
    float light = 0.;
    vec3 n = getnormal(ro);
    vec3 ld = normalize(lp - ro);
    float lightray = raymarch(lp, -ld);
    float shadow = lightray.x >= length(lp - ro) ? 1 : 0.2;

    light = dot(ld, n);
    light *= shadow;
    return light;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy-0.5*u_resolution.xy) / u_resolution.y;
    vec3 skycol = vec3(0.3, 0.7, 0.9);
    vec3 ro = vec3(0, 2, -5);
    vec3 rd = normalize(vec3(uv.x, uv.y, 0.5));

    vec2 mouse = -((u_mouse/u_resolution)-0.5) * PI * M_SPEED;
    rd.yz *= rot(0.3);
    rd.yz *= rot(mouse.y);
    rd.xz *= rot(mouse.x);
    ro.yz *= rot(mouse.y);
    ro.xz *= rot(mouse.x);

    vec3 lightpos = vec3(5, 10., -6);

    float dist = raymarch(ro,rd);
    vec3 hit = ro+rd*(dist-2.*EPSILON);
    float light = getlight(hit, lightpos);

    vec3 col = vec3(1);
    col *= light;
    

    if (dist >= MAX_DIST) col = skycol;

    fragColor = vec4(col, 1.0);
}