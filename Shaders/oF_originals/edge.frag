#version 120
uniform sampler2DRect tex;
uniform vec2 resolution;
uniform float strength;
uniform float mixAmount;

void main() {
    vec2 uv = gl_TexCoord[0].st;

    // 3x3 Sobel kernel
    vec3 tl = texture2DRect(tex, uv + vec2(-1, -1)).rgb;
    vec3 tc = texture2DRect(tex, uv + vec2( 0, -1)).rgb;
    vec3 tr = texture2DRect(tex, uv + vec2( 1, -1)).rgb;
    vec3 ml = texture2DRect(tex, uv + vec2(-1,  0)).rgb;
    vec3 mc = texture2DRect(tex, uv).rgb;
    vec3 mr = texture2DRect(tex, uv + vec2( 1,  0)).rgb;
    vec3 bl = texture2DRect(tex, uv + vec2(-1,  1)).rgb;
    vec3 bc = texture2DRect(tex, uv + vec2( 0,  1)).rgb;
    vec3 br = texture2DRect(tex, uv + vec2( 1,  1)).rgb;

    vec3 gx = -tl - 2.0*ml - bl + tr + 2.0*mr + br;
    vec3 gy = -tl - 2.0*tc - tr + bl + 2.0*bc + br;

    float edge = length(sqrt(gx * gx + gy * gy));

    // neon-style: use original color * edge intensity
    vec3 edgeColor = mc * edge * strength;
    vec3 result = mix(edgeColor, mc, mixAmount);

    gl_FragColor = vec4(result, 1.0);
}
