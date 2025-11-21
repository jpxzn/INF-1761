#version 410

layout(vertices = 4) out;

uniform float tessLevelInner = 8.0;
uniform float tessLevelOuter = 8.0;

void main() {
    // Pass-through control points
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Set tessellation levels once per patch
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = tessLevelInner;
        gl_TessLevelInner[1] = tessLevelInner;
        gl_TessLevelOuter[0] = tessLevelOuter;
        gl_TessLevelOuter[1] = tessLevelOuter;
        gl_TessLevelOuter[2] = tessLevelOuter;
        gl_TessLevelOuter[3] = tessLevelOuter;
    }
}
