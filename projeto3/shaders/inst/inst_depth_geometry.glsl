#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 128) out;

uniform mat4 LightView;
uniform mat4 LightProj;

in VS_OUT {
    vec3 pos;
    vec3 scale;
    float typeId;
    vec3 color;
} gs_in[];

void emitTri(vec3 a, vec3 b, vec3 c){
    vec3 p_world_a = a * gs_in[0].scale + gs_in[0].pos;
    vec3 p_world_b = b * gs_in[0].scale + gs_in[0].pos;
    vec3 p_world_c = c * gs_in[0].scale + gs_in[0].pos;
    
    gl_Position = LightProj * LightView * vec4(p_world_a, 1.0);
    EmitVertex();
    gl_Position = LightProj * LightView * vec4(p_world_b, 1.0);
    EmitVertex();
    gl_Position = LightProj * LightView * vec4(p_world_c, 1.0);
    EmitVertex();
    EndPrimitive();
}

void emitCube(){
    vec3 p[8] = vec3[8](
        vec3(-0.5, 0.0,-0.5), vec3(0.5, 0.0,-0.5), vec3(0.5, 1.0,-0.5), vec3(-0.5, 1.0,-0.5),
        vec3(-0.5, 0.0, 0.5), vec3(0.5, 0.0, 0.5), vec3(0.5, 1.0, 0.5), vec3(-0.5, 1.0, 0.5)
    );
    emitTri(p[0],p[1],p[2]); emitTri(p[0],p[2],p[3]);
    emitTri(p[4],p[6],p[5]); emitTri(p[4],p[7],p[6]);
    emitTri(p[0],p[3],p[7]); emitTri(p[0],p[7],p[4]);
    emitTri(p[1],p[5],p[6]); emitTri(p[1],p[6],p[2]);
    emitTri(p[0],p[4],p[5]); emitTri(p[0],p[5],p[1]);
    emitTri(p[3],p[2],p[6]); emitTri(p[3],p[6],p[7]);
}

void emitOcta(){
    vec3 top = vec3(0, 1.0, 0);
    vec3 bot = vec3(0, 0.0, 0);
    vec3 v[4] = vec3[4](vec3(-0.5, 0.5, 0), vec3(0, 0.5, 0.5), vec3(0.5, 0.5, 0), vec3(0, 0.5,-0.5));
    emitTri(top, v[0], v[1]); emitTri(top, v[1], v[2]); emitTri(top, v[2], v[3]); emitTri(top, v[3], v[0]);
    emitTri(bot, v[1], v[0]); emitTri(bot, v[2], v[1]); emitTri(bot, v[3], v[2]); emitTri(bot, v[0], v[3]);
}

void emitHexCylinder(){
    const int N = 6; float angStep = 6.2831853/float(N);
    vec3 top[N]; vec3 bot[N];
    for (int i=0;i<N;++i){ float a=i*angStep; float x=0.5*cos(a), z=0.5*sin(a); top[i]=vec3(x, 1.0, z); bot[i]=vec3(x, 0.0, z);}    
    for (int i=0;i<N;++i){ int j=(i+1)%N; emitTri(top[i], bot[i], bot[j]); emitTri(top[i], bot[j], top[j]); }
    for (int i=1;i<N-1;++i){ emitTri(top[0], top[i], top[i+1]); }
    for (int i=1;i<N-1;++i){ emitTri(bot[0], bot[i+1], bot[i]); }
}

void emitCone(){
    const int N = 8; float angStep = 6.2831853/float(N);
    vec3 apex = vec3(0, 1.0, 0);
    vec3 base[N];
    for (int i=0; i<N; ++i) { float a=i*angStep; base[i]=vec3(0.5*cos(a), 0.0, 0.5*sin(a)); }
    for (int i=0; i<N; ++i) { int j=(i+1)%N; emitTri(apex, base[i], base[j]); }
    for (int i=1; i<N-1; ++i) { emitTri(base[0], base[i+1], base[i]); }
}

void main(){
    int t = int(gs_in[0].typeId + 0.5);
    if (t==0) emitCube();
    else if (t==1) emitHexCylinder();
    else if (t==2) emitCone();
    else emitOcta();
}
