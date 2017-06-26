#version 330 core

in vec3 eyeSpaceVertex;

out vec4 fragColor;

void main() {
    vec3 normal = cross(dFdx(eyeSpaceVertex), dFdy(eyeSpaceVertex));
    fragColor = vec4(0.5)*normalize(normal).z;
}
