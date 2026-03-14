#version 120
uniform sampler2DRect tex;
uniform float threshold;

void main() {
    vec4  color = texture2DRect(tex, gl_TexCoord[0].st);
    float luma  = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    gl_FragColor = (luma >= threshold) ? color : vec4(0.0);
}
