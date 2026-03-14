#version 120
uniform sampler2DRect tex;
uniform vec2 resolution;
uniform float time;
uniform float intensity;
uniform float blockSize;

float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

void main() {
    vec2 uv = gl_TexCoord[0].st;

    // block-based horizontal shift
    float block = floor(uv.y / blockSize);
    float seed  = block * 0.37 + floor(time * 8.0) * 1.13;
    float shift = (hash(seed) - 0.5) * 2.0 * intensity * resolution.x * 0.1;

    // only shift some blocks (60% chance)
    float active = step(0.4, hash(seed + 7.0));
    shift *= active;

    vec2 tc = uv;
    tc.x += shift;
    tc.x = clamp(tc.x, 0.0, resolution.x);

    // RGB split on shifted blocks
    float rgbShift = shift * 0.3;
    float r = texture2DRect(tex, vec2(tc.x + rgbShift, tc.y)).r;
    float g = texture2DRect(tex, tc).g;
    float b = texture2DRect(tex, vec2(tc.x - rgbShift, tc.y)).b;

    gl_FragColor = vec4(r, g, b, 1.0);
}
