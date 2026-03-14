#version 120
uniform sampler2DRect tex;
uniform vec2 resolution;
uniform float scanlineSpacing;
uniform float curvature;
uniform float chromatic;
uniform float vignetteAmount;
uniform float noiseAmount;
uniform float time;

// simple pseudo-random
float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = gl_TexCoord[0].st;
    vec2 ndc = uv / resolution;          // 0..1

    // ── barrel distortion ──
    vec2 c = ndc - 0.5;
    float r2 = dot(c, c);
    vec2 distorted = ndc + c * r2 * curvature;
    vec2 tc = distorted * resolution;

    // out-of-bounds → black
    if (distorted.x < 0.0 || distorted.x > 1.0 ||
        distorted.y < 0.0 || distorted.y > 1.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // ── chromatic aberration (RGB split) ──
    float rr = texture2DRect(tex, tc + vec2( chromatic, 0.0)).r;
    float gg = texture2DRect(tex, tc).g;
    float bb = texture2DRect(tex, tc + vec2(-chromatic, 0.0)).b;
    vec3 col = vec3(rr, gg, bb);

    // ── scanlines ──
    float scanline = 1.0;
    if (mod(gl_FragCoord.y, scanlineSpacing) < 1.0) scanline = 0.7;
    col *= scanline;

    // ── vignette ──
    float vig = 1.0 - dot(c, c) * (vignetteAmount * 4.0);
    col *= clamp(vig, 0.0, 1.0);

    // ── noise ──
    float n = rand(uv * 0.01 + vec2(time)) * noiseAmount;
    col += vec3(n);

    gl_FragColor = vec4(col, 1.0);
}
