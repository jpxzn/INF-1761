#version 410 core

in vec3 vEye;
in vec3 nEye;
out vec4 fcolor;

uniform vec4 lpos;
uniform vec4 lamb, ldif, lspe;

uniform vec4 mamb, mdif, mspe;
uniform float mshi;

void main() {
    vec3 N = normalize(nEye);
    vec3 L = (lpos.w == 0.0) ? normalize(lpos.xyz)
                             : normalize(lpos.xyz - vEye);
    vec3 V = normalize(-vEye);
    vec3 R = reflect(-L, N);

    float NdotL = max(dot(N, L), 0.0);

    vec4 ambient  = mamb * lamb;
    vec4 diffuse  = mdif * ldif * NdotL;
    vec4 specular = (NdotL > 0.0) ? (mspe * lspe * pow(max(dot(R, V), 0.0), mshi))
                                  : vec4(0.0);

    fcolor = ambient + diffuse + specular;
}
