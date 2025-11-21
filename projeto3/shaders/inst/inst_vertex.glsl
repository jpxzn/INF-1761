#version 410

// Per-instance attributes
layout(location = 4) in vec3 i_pos;    // instance position (object space offset)
layout(location = 5) in vec3 i_scale;  // instance scale per axis
layout(location = 6) in float i_type;  // 0=cube, 1=cyl, 2=sphere (octa)
layout(location = 7) in vec3 i_color;  // base color multiplier

// Pass-through to geometry shader
out VS_OUT {
    vec3 pos;
    vec3 scale;
    float typeId;
    vec3 color;
} vs_out;

void main() {
    vs_out.pos = i_pos;
    vs_out.scale = i_scale;
    vs_out.typeId = i_type;
    vs_out.color = i_color;
    // GS will generate actual positions, but we need to set something
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
