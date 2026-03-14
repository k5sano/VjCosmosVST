#version 120
uniform sampler2DRect baseTex;   // 元の描画
uniform sampler2DRect bloomTex;  // ブラー済み輝度
uniform float bloomIntensity;

void main() {
    vec2 uv    = gl_TexCoord[0].st;
    vec4 base  = texture2DRect(baseTex,  uv);
    vec4 bloom = texture2DRect(bloomTex, uv);
    gl_FragColor = base + bloom * bloomIntensity;
}
