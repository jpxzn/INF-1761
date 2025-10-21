#version 410

in VS_OUT {
  vec3 veye;
  vec3 neye;
} f;

out vec4 fcolor;

uniform vec4 lpos;  // light pos in eye space
uniform vec4 lamb;
uniform vec4 ldif;
uniform vec4 lspe;

uniform vec4 mamb;
uniform vec4 mdif;
uniform vec4 mspe;
uniform float mshi;

void main (void)
{
  vec3 light;
  if (lpos.w == 0) 
    light = normalize(vec3(lpos));
  else 
    light = normalize(vec3(lpos)-f.veye);
  vec3 neye = normalize(f.neye);
  float ndotl = max(0.0, dot(neye, light));
  vec4 col = mamb*lamb + mdif * ldif * ndotl;
  if (ndotl > 0.0) {
    vec3 refl = normalize(reflect(-light,neye));
    col += mspe * lspe * pow(max(0.0, dot(refl, normalize(-f.veye))), mshi);
  }
  fcolor = col;
}

