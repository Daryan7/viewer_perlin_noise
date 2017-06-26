#version 330 core

layout (location = 0) in vec2 vertex;
layout (location = 1) in float noise;

out vec3 eyeSpaceVertex;
out vec3 objectSpaceVertex;
out vec2 vtexCoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

void main() {
    vtexCoord = vertex;
	float z = mix(-10, 10, noise);
    vec3 V = vec3(vertex, z);
    objectSpaceVertex = V;
	eyeSpaceVertex = (modelViewMatrix*vec4(V, 1)).xyz;
    gl_Position = modelViewProjectionMatrix*vec4(V, 1);
}
