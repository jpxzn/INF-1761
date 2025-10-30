#include "cylinder.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <cmath>
#include "disk.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI glm::pi<float>()

CylinderPtr Cylinder::Make (int nstack, int nslice, float height, float radius) {
    return CylinderPtr(new Cylinder(nstack, nslice, height, radius));
}

Cylinder::Cylinder (int nstack, int nslice, float height, float radius)
    : _height(height) {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> texcoords;
    std::vector<unsigned int> indices;

    // --- Geração dos vértices para o cilindro ---
    for (int i = 0; i <= nstack; ++i) {
        float y = (float(i) / nstack - 0.5f) * height;
        float v = float(i) / nstack;

        for (int j = 0; j <= nslice; ++j) {
            float theta = float(j) / nslice * 2 * PI;
            float x = radius * cos(theta);
            float z = radius * sin(theta);

            // Posição
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal (radial)
            normals.push_back(cos(theta));
            normals.push_back(0.0f);
            normals.push_back(sin(theta));

            // Tangente (em torno da circunferência)
            tangents.push_back(-sin(theta));
            tangents.push_back(0.0f);
            tangents.push_back(cos(theta));

            // Coordenadas de textura
            texcoords.push_back(float(j) / nslice);
            texcoords.push_back(v);
        }
    }

    // --- Índices para triângulos ---
    for (int i = 0; i < nstack; ++i) {
        for (int j = 0; j < nslice; ++j) {
            int row1 = i * (nslice + 1);
            int row2 = (i + 1) * (nslice + 1);

            indices.push_back(row1 + j);
            indices.push_back(row2 + j);
            indices.push_back(row2 + j + 1);

            indices.push_back(row1 + j);
            indices.push_back(row2 + j + 1);
            indices.push_back(row1 + j + 1);
        }
    }

    m_nind = indices.size();

    // --- Upload para GPU ---
    GLuint id[4];
    GLuint index;
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(4, id);
    glBindBuffer(GL_ARRAY_BUFFER, id[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Shape::LOC::COORD, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(Shape::LOC::COORD);

    glBindBuffer(GL_ARRAY_BUFFER, id[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Shape::LOC::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(Shape::LOC::NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, id[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tangents.size(), tangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Shape::LOC::TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(Shape::LOC::TANGENT);

    glBindBuffer(GL_ARRAY_BUFFER, id[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(Shape::LOC::TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(Shape::LOC::TEXCOORD);

    glGenBuffers(1, &index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // --- Criar o Tampo superior e inferior ---
    m_top = Disk::Make(nslice, radius);  // Criando o tampo superior
    m_bottom = Disk::Make(nslice, radius);  // Criando o tampo inferior
}

Cylinder::~Cylinder() {
    if (m_vao)
        glDeleteVertexArrays(1, &m_vao);
}

void Cylinder::Draw(StatePtr st) {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_nind, GL_UNSIGNED_INT, 0);

    // ---------- Tampo superior ----------
    st->PushMatrix();
    // Cria a transformação para o tampo superior
    glm::mat4 Mtop = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f * _height, 0.0f)) 
                     * glm::rotate(glm::mat4(1.0f), -PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotaciona ao longo do eixo X
    st->MultMatrix(Mtop); // Aplica a transformação
    st->LoadMatrices();   // Atualiza as matrizes no shader
    m_top->Draw(st);      // Desenha o tampo superior
    st->PopMatrix();      // Restaura a transformação anterior

    // ---------- Tampo inferior ----------
    st->PushMatrix();
    // Cria a transformação para o tampo inferior
    glm::mat4 Mbot = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f * _height, 0.0f)) 
                     * glm::rotate(glm::mat4(1.0f), PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotaciona ao longo do eixo X
    st->MultMatrix(Mbot); // Aplica a transformação
    st->LoadMatrices();   // Atualiza as matrizes no shader
    m_bottom->Draw(st);   // Desenha o tampo inferior
    st->PopMatrix();      // Restaura a transformação anterior
}