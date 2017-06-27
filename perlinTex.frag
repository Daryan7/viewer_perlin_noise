#version 330 core

uniform sampler2D tex;

in vec2 vtexCoord;
out vec4 fragColor;

void main() {
    float noise = texture(tex, vtexCoord).r;
    fragColor = vec4(noise);
}
