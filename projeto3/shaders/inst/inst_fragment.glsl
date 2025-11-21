#version 410

in GS_OUT {
    vec4 color;
    vec4 lightPos;
} fs_in;

out vec4 fcolor;

void main() {
    fcolor = fs_in.color;
}
