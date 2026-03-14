#version 120
uniform sampler2DRect tex;
uniform float mode;
uniform float intensity;

void main() {
    vec2 uv = gl_TexCoord[0].st;
    vec3 col = texture2DRect(tex, uv).rgb;

    float lum = dot(col, vec3(0.299, 0.587, 0.114));
    vec3 mono  = vec3(lum);
    vec3 sepia = vec3(lum * 1.2, lum * 1.0, lum * 0.8);

    vec3 tinted = mix(mono, sepia, mode);
    vec3 result = mix(col, tinted, intensity);

    gl_FragColor = vec4(result, 1.0);
}
