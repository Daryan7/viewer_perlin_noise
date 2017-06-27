#version 330 core

layout (location = 0) in vec3 vertex;

out vec2 vtexCoord;

uniform mat4 modelViewProjectionMatrix;

void main() {
    vtexCoord = vertex.xy;
    gl_Position = vec4((vertex.xy*0.3*2)-1,0,1);
}
