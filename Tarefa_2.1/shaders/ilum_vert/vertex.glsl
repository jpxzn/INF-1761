#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mv; 
uniform mat4 Mn; 
uniform mat4 Mvp;

// Pass position and normal in eye space to the fragment shader
out VS_OUT {
  vec3 veye;
  vec3 neye;
} v;

void main (void) 
{
  v.veye = vec3(Mv*coord);
  v.neye = normalize(vec3(Mn*vec4(normal,0.0f)));
  gl_Position = Mvp*coord; 
}

