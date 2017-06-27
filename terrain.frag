#version 330 core

uniform sampler2D grass;
uniform sampler2D rock;

in vec3 objectSpaceVertex;
in vec3 eyeSpaceVertex;
in vec2 vtexCoord;

out vec4 fragColor;

float fade(float t) {
    return t*t*t*t;
}

void main() {
    vec4 colGrass = texture(grass, vtexCoord);
    vec4 colRock = texture(rock, vtexCoord);
    vec3 normalO = cross(dFdx(objectSpaceVertex), dFdy(objectSpaceVertex));
    vec3 normal = cross(dFdx(eyeSpaceVertex), dFdy(eyeSpaceVertex));
    fragColor = mix(colRock, colGrass, fade(normalO.z/length(normalO)))*normalize(normal).z;
}
