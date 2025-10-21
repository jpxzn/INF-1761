#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 texcoord;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

out VS_OUT {
  vec3 veye;
  vec3 neye;
  vec2 uv;
} v;

void main (void) 
{
  v.veye = vec3(Mv*coord);
  v.neye = normalize(vec3(Mn*vec4(normal,0.0f)));
  v.uv = texcoord;
  gl_Position = Mvp*coord; 
}

