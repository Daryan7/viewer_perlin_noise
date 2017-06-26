#version 330 core

layout (location = 0) in vec2 vertex;
layout (location = 1) in float noise;

out vec3 eyeSpaceVertex;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

void main() {
	float z = mix(-10, 10, noise);
	eyeSpaceVertex = (modelViewMatrix*vec4(vertex, z, 1)).xyz;
    gl_Position = modelViewProjectionMatrix*vec4(vertex, z, 1);
}
