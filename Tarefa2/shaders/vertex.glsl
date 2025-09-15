#version 410

layout (location=0) in vec2 vertex;
layout (location=1) in vec3 icolor;

out vec4 color;

uniform mat4 uModel;

void main(void) {
  color = vec4(icolor, 1.0);
  gl_Position = uModel * vec4(vertex, 0.0, 1.0);
}