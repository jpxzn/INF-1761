#version 410

layout(quads, equal_spacing, ccw) in;

uniform mat4 Mv;      // model matrix (world lighting space)
uniform mat4 Mn;      // normal matrix
uniform mat4 Mvp;     // camera MVP
uniform mat4 LightView;
uniform mat4 LightProj;

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
    vec4 lightPos;
} vs_out;

// Map quad domain (u,v in [0,1]^2) to a unit sphere via spherical coords
vec3 sphereFromUV(vec2 uv) {
    float theta = uv.x * 6.28318530718; // 2*pi
    float phi   = uv.y * 3.14159265359; // pi
    vec3 p;
    p.x = sin(phi) * cos(theta);
    p.y = cos(phi);
    p.z = sin(phi) * sin(theta);
    return p;
}

void main() {
    // Compute (u,v) from tess coords for a quad
    vec2 uv = vec2(gl_TessCoord.x, gl_TessCoord.y);

    // Unit sphere in object space; scale/translate via node Transform on CPU
    vec3 pos_obj = sphereFromUV(uv);
    vec3 n_obj = normalize(pos_obj);

    // World-space position/normal for lighting
    vec3 veye = vec3(Mv * vec4(pos_obj,1.0));
    vec3 neye = normalize(vec3(Mn * vec4(n_obj,0.0)));

    // Phong lighting in world space
    vec3 lightDir = (lpos.w == 0.0) ? normalize(vec3(lpos)) : normalize(vec3(lpos) - veye);
    float ndotl = max(0.0, dot(neye, lightDir));
    vec4 color = mamb * lamb + mdif * ldif * ndotl;
    if (ndotl > 0.0) {
        vec3 refl = normalize(reflect(-lightDir, neye));
        color += mspe * lspe * pow(max(0.0, dot(refl, normalize(-veye))), mshi);
    }

    vs_out.color = color;
    vs_out.lightPos = LightProj * LightView * Mv * vec4(pos_obj,1.0);
    gl_Position = Mvp * vec4(pos_obj,1.0);
}
