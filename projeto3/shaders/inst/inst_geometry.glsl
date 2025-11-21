#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 128) out;

// Matrices and lighting
uniform mat4 Mv;
uniform mat4 Mn;
uniform mat4 Mvp;
uniform mat4 LightView;
uniform mat4 LightProj;

uniform vec4 lpos;
uniform vec4 lamb;
uniform vec4 ldif;
uniform vec4 lspe;

uniform vec4 mamb;
uniform vec4 mdif;
uniform vec4 mspe;
uniform float mshi;

in VS_OUT {
    vec3 pos;
    vec3 scale;
    float typeId;
    vec3 color;
} gs_in[];

out GS_OUT {
    vec4 color;
    vec4 lightPos;
} gs_out;

void setVertex(vec3 p_obj, vec3 n_obj, vec3 baseColor) {
    // Transform to world position: scale then translate
    vec3 p_world = p_obj * gs_in[0].scale + gs_in[0].pos;
    vec3 n_world = normalize(n_obj);
    
    // Lighting in WORLD space (mode="world" means lpos is in world coords)
    vec3 lightDir;
    if (lpos.w == 0.0) {
        // Directional light
        lightDir = normalize(vec3(lpos));
    } else {
        // Point light
        lightDir = normalize(vec3(lpos) - p_world);
    }
    
    float ndotl = max(0.0, dot(n_world, lightDir));
    
    // Simple lighting with fixed coefficients (ignore material uniforms for instanced objects)
    vec4 col = vec4(baseColor, 1.0);
    vec4 ambient = vec4(0.3) * col;  // 30% ambient
    vec4 diffuse = vec4(0.7) * ndotl * col;  // 70% diffuse
    vec4 phong = ambient + diffuse;
    
    // Clamp to reasonable values
    phong = clamp(phong, 0.0, 1.0);
    
    gs_out.color = phong;
    gs_out.lightPos = LightProj * LightView * vec4(p_world, 1.0);
    gl_Position = Mvp * vec4(p_world, 1.0);
    EmitVertex();
}

void emitTriangle(vec3 a, vec3 b, vec3 c, vec3 na, vec3 nb, vec3 nc, vec3 baseColor) {
    setVertex(a, na, baseColor);
    setVertex(b, nb, baseColor);
    setVertex(c, nc, baseColor);
    EndPrimitive();
}

void emitCube(vec3 col) {
    // axis-aligned cube from 0..1 in Y (base at origin)
    vec3 p[8] = vec3[8](
        vec3(-0.5, 0.0,-0.5), vec3(0.5, 0.0,-0.5), vec3(0.5, 1.0,-0.5), vec3(-0.5, 1.0,-0.5),
        vec3(-0.5, 0.0, 0.5), vec3(0.5, 0.0, 0.5), vec3(0.5, 1.0, 0.5), vec3(-0.5, 1.0, 0.5)
    );
    // faces: each two triangles
    // -Z
    emitTriangle(p[0],p[1],p[2], vec3(0,0,-1),vec3(0,0,-1),vec3(0,0,-1), col);
    emitTriangle(p[0],p[2],p[3], vec3(0,0,-1),vec3(0,0,-1),vec3(0,0,-1), col);
    // +Z
    emitTriangle(p[4],p[6],p[5], vec3(0,0,1),vec3(0,0,1),vec3(0,0,1), col);
    emitTriangle(p[4],p[7],p[6], vec3(0,0,1),vec3(0,0,1),vec3(0,0,1), col);
    // -X
    emitTriangle(p[0],p[3],p[7], vec3(-1,0,0),vec3(-1,0,0),vec3(-1,0,0), col);
    emitTriangle(p[0],p[7],p[4], vec3(-1,0,0),vec3(-1,0,0),vec3(-1,0,0), col);
    // +X
    emitTriangle(p[1],p[5],p[6], vec3(1,0,0),vec3(1,0,0),vec3(1,0,0), col);
    emitTriangle(p[1],p[6],p[2], vec3(1,0,0),vec3(1,0,0),vec3(1,0,0), col);
    // -Y
    emitTriangle(p[0],p[4],p[5], vec3(0,-1,0),vec3(0,-1,0),vec3(0,-1,0), col);
    emitTriangle(p[0],p[5],p[1], vec3(0,-1,0),vec3(0,-1,0),vec3(0,-1,0), col);
    // +Y
    emitTriangle(p[3],p[2],p[6], vec3(0,1,0),vec3(0,1,0),vec3(0,1,0), col);
    emitTriangle(p[3],p[6],p[7], vec3(0,1,0),vec3(0,1,0),vec3(0,1,0), col);
}

void emitOcta(vec3 col) {
    // Octahedron with base at origin, top at y=1.0
    vec3 top = vec3(0, 1.0, 0);
    vec3 bot = vec3(0, 0.0, 0);
    vec3 v[4] = vec3[4](vec3(-0.5, 0.5, 0), vec3(0, 0.5, 0.5), vec3(0.5, 0.5, 0), vec3(0, 0.5,-0.5));
    // upper
    emitTriangle(top, v[0], v[1], normalize(cross(v[0]-top, v[1]-top)), normalize(cross(v[0]-top, v[1]-top)), normalize(cross(v[0]-top, v[1]-top)), col);
    emitTriangle(top, v[1], v[2], normalize(cross(v[1]-top, v[2]-top)), normalize(cross(v[1]-top, v[2]-top)), normalize(cross(v[1]-top, v[2]-top)), col);
    emitTriangle(top, v[2], v[3], normalize(cross(v[2]-top, v[3]-top)), normalize(cross(v[2]-top, v[3]-top)), normalize(cross(v[2]-top, v[3]-top)), col);
    emitTriangle(top, v[3], v[0], normalize(cross(v[3]-top, v[0]-top)), normalize(cross(v[3]-top, v[0]-top)), normalize(cross(v[3]-top, v[0]-top)), col);
    // lower
    emitTriangle(bot, v[1], v[0], normalize(cross(v[1]-bot, v[0]-bot)), normalize(cross(v[1]-bot, v[0]-bot)), normalize(cross(v[1]-bot, v[0]-bot)), col);
    emitTriangle(bot, v[2], v[1], normalize(cross(v[2]-bot, v[1]-bot)), normalize(cross(v[2]-bot, v[1]-bot)), normalize(cross(v[2]-bot, v[1]-bot)), col);
    emitTriangle(bot, v[3], v[2], normalize(cross(v[3]-bot, v[2]-bot)), normalize(cross(v[3]-bot, v[2]-bot)), normalize(cross(v[3]-bot, v[2]-bot)), col);
    emitTriangle(bot, v[0], v[3], normalize(cross(v[0]-bot, v[3]-bot)), normalize(cross(v[0]-bot, v[3]-bot)), normalize(cross(v[0]-bot, v[3]-bot)), col);
}

void emitHexCylinder(vec3 col) {
    // hexagonal prism radius 0.5 height 1.0 base at origin
    const int N = 6;
    float angStep = 6.2831853/float(N);
    vec3 top[N]; vec3 bot[N];
    for (int i=0;i<N;++i){
        float a = i*angStep;
        float x = 0.5*cos(a), z = 0.5*sin(a);
        top[i] = vec3(x, 1.0, z);
        bot[i] = vec3(x, 0.0, z);
    }
    // sides
    for (int i=0;i<N;++i){
        int j = (i+1)%N;
        vec3 n = normalize(vec3(top[i].x+top[j].x, 0.0, top[i].z+top[j].z));
        emitTriangle(top[i], bot[i], bot[j], n,n,n, col);
        emitTriangle(top[i], bot[j], top[j], n,n,n, col);
    }
    // top (fan to center)
    for (int i=1;i<N-1;++i){
        emitTriangle(top[0], top[i], top[i+1], vec3(0,1,0),vec3(0,1,0),vec3(0,1,0), col);
    }
    // bottom
    for (int i=1;i<N-1;++i){
        emitTriangle(bot[0], bot[i+1], bot[i], vec3(0,-1,0),vec3(0,-1,0),vec3(0,-1,0), col);
    }
}

void emitCone(vec3 col) {
    // Cone with base at origin, apex at y=1.0
    const int N = 8;
    float angStep = 6.2831853/float(N);
    vec3 apex = vec3(0, 1.0, 0);
    vec3 base[N];
    for (int i=0; i<N; ++i) {
        float a = i * angStep;
        base[i] = vec3(0.5*cos(a), 0.0, 0.5*sin(a));
    }
    // sides
    for (int i=0; i<N; ++i) {
        int j = (i+1) % N;
        vec3 edge1 = base[i] - apex;
        vec3 edge2 = base[j] - apex;
        vec3 n = normalize(cross(edge1, edge2));
        emitTriangle(apex, base[i], base[j], n, n, n, col);
    }
    // base
    for (int i=1; i<N-1; ++i) {
        emitTriangle(base[0], base[i+1], base[i], vec3(0,-1,0), vec3(0,-1,0), vec3(0,-1,0), col);
    }
}

void main() {
    int t = int(gs_in[0].typeId + 0.5);
    vec3 col = gs_in[0].color;
    if (t == 0) emitCube(col);
    else if (t == 1) emitHexCylinder(col);
    else if (t == 2) emitCone(col);
    else emitOcta(col);
}
