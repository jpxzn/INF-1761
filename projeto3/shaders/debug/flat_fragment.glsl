#version 410
in vec3 vN;
out vec4 fcolor;
uniform vec3 baseColor = vec3(0.2,0.8,0.3);
void main(){
  float a = 0.4 + 0.6*abs(vN.z); // simple variation so we see normals
  fcolor = vec4(baseColor * a, 1.0);
}
