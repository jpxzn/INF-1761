#version 410

layout(location = 0) in vec4 coord;

uniform mat4 Mvp; // provided by rendering with light camera

void main() {
    gl_Position = Mvp * coord;
}
