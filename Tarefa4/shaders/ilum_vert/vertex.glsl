#version 410 core

layout(location = 0) in vec3 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mv;
uniform mat4 Mn;
uniform mat4 Mvp;

out vec3 vEye;
out vec3 nEye;

void main() {
    vec4 posEye = Mv * vec4(coord, 1.0);
    vEye = posEye.xyz;

    nEye = normalize( vec3(Mn * vec4(normal, 0.0)) );

    gl_Position = Mvp * vec4(coord, 1.0);
}