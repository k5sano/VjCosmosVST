#version 120
uniform sampler2DRect tex;
uniform vec2 resolution;
uniform int segments;
uniform float rotation;
uniform float zoom;

void main() {
    vec2 uv = gl_TexCoord[0].st;
    vec2 center = resolution * 0.5;

    // center-relative coordinates
    vec2 p = uv - center;

    // to polar
    float r = length(p) / zoom;
    float a = atan(p.y, p.x) + rotation;

    // kaleidoscope fold
    float segAngle = 3.14159265 / float(segments);
    a = mod(a, segAngle * 2.0);
    if (a > segAngle) a = segAngle * 2.0 - a;

    // back to cartesian (texture coords)
    vec2 tc = center + vec2(cos(a), sin(a)) * r;

    // clamp to texture bounds
    tc = clamp(tc, vec2(0.0), resolution);

    gl_FragColor = texture2DRect(tex, tc);
}
