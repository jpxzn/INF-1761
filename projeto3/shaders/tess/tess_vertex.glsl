#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal; // unused here

// Pass-through of control points; uniforms are used in TES
void main() {
    // Keep input position as control point; TES will parametrize surface
    gl_Position = coord;
}
