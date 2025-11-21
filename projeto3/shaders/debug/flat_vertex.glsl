#version 410
layout(location=0) in vec4 coord;
layout(location=1) in vec3 normal; // ignored but keeps VAO compatibility
uniform mat4 Mvp;
uniform mat4 Mv;
uniform mat4 Mn;
out vec3 vN;
void main(){
  vN = normalize(vec3(Mn * vec4(normal,0.0)));
  gl_Position = Mvp * coord;
}
