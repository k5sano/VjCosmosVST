#version 120
uniform sampler2DRect tex;
uniform vec2 resolution;
uniform float mode;

void main() {
    vec2 uv = gl_TexCoord[0].st;
    int m = int(mode);

    if (m == 0 || m == 2) {
        // left-right mirror
        if (uv.x > resolution.x * 0.5)
            uv.x = resolution.x - uv.x;
    }
    if (m == 1 || m == 2) {
        // top-bottom mirror
        if (uv.y > resolution.y * 0.5)
            uv.y = resolution.y - uv.y;
    }

    gl_FragColor = texture2DRect(tex, uv);
}
