#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mv;      // model matrix (since lighting space 'world')
uniform mat4 Mn;      // normal matrix (not used for shadow coords but for lighting)
uniform mat4 Mvp;     // regular camera MVP
uniform mat4 LightView;  // light view matrix
uniform mat4 LightProj;  // light projection matrix

// light uniforms (Phong)
uniform vec4 lpos;
uniform vec4 lamb;
uniform vec4 ldif;
uniform vec4 lspe;

uniform vec4 mamb;
uniform vec4 mdif;
uniform vec4 mspe;
uniform float mshi;

out VS_OUT {
    vec4 color;
    vec4 lightPos; // position in light clip space
} vs_out;

void main() {
    // Phong lighting in world space
    vec3 veye = vec3(Mv * coord); // world position
    vec3 lightDir;
    if (lpos.w == 0.0)
        lightDir = normalize(vec3(lpos));
    else
        lightDir = normalize(vec3(lpos) - veye);
    vec3 neye = normalize(vec3(Mn * vec4(normal,0.0)));
    float ndotl = dot(neye, lightDir);
    vs_out.color = mamb * lamb + mdif * ldif * max(0.0, ndotl);
    if (ndotl > 0.0) {
        vec3 refl = normalize(reflect(-lightDir, neye));
        vs_out.color += mspe * lspe * pow(max(0.0, dot(refl, normalize(-veye))), mshi);
    }

    // light clip position (no bias here; handled in fragment)
    vs_out.lightPos = LightProj * LightView * Mv * coord;
    gl_Position = Mvp * coord;
}
