#version 120
uniform sampler2DRect tex;
uniform vec2 resolution;
uniform float time;
uniform float amplitude;
uniform float frequency;
uniform float speed;

void main() {
    vec2 uv = gl_TexCoord[0].st;
    uv.x += sin(uv.y * frequency + time * speed) * amplitude;
    uv.y += cos(uv.x * frequency + time * speed * 0.7) * amplitude * 0.5;
    uv = clamp(uv, vec2(0.0), resolution);
    gl_FragColor = texture2DRect(tex, uv);
}
