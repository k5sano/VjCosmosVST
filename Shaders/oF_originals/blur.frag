#version 120
uniform sampler2DRect tex;
uniform vec2  direction;    // (1,0)=水平  (0,1)=垂直
uniform float blurAmount;   // サンプル間隔 (px)

void main() {
    vec2 uv = gl_TexCoord[0].st;

    // 9-tap Gaussian (sigma≈2, 正規化済み)
    vec4 c = vec4(0.0);
    c += texture2DRect(tex, uv + direction * -4.0 * blurAmount) * 0.0276;
    c += texture2DRect(tex, uv + direction * -3.0 * blurAmount) * 0.0663;
    c += texture2DRect(tex, uv + direction * -2.0 * blurAmount) * 0.1238;
    c += texture2DRect(tex, uv + direction * -1.0 * blurAmount) * 0.1802;
    c += texture2DRect(tex, uv                                ) * 0.2042;
    c += texture2DRect(tex, uv + direction *  1.0 * blurAmount) * 0.1802;
    c += texture2DRect(tex, uv + direction *  2.0 * blurAmount) * 0.1238;
    c += texture2DRect(tex, uv + direction *  3.0 * blurAmount) * 0.0663;
    c += texture2DRect(tex, uv + direction *  4.0 * blurAmount) * 0.0276;

    gl_FragColor = c;
}
