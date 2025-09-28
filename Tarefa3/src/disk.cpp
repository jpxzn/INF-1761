#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "disk.h"

Disk::Disk(int nslice) : m_vao(0), m_nslice(nslice) {

    std::vector<glm::vec2> verts;
    verts.reserve(m_nslice + 2);
    verts.push_back(glm::vec2(0.0f, 0.0f));

    for (int i = 0; i <= m_nslice; ++i) {
        float ang = float(i) * glm::two_pi<float>() / float(m_nslice);
        glm::vec2 p(std::cos(ang), std::sin(ang));
        verts.push_back(p);
    }

    GLuint vbo = 0;
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * verts.size(), verts.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(Shape::LOC::COORD);
    glVertexAttribPointer(Shape::LOC::COORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glBindVertexArray(0);
}

Disk::~Disk() {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

DiskPtr Disk::Make(int nslice) {
    return DiskPtr(new Disk(nslice));
}

void Disk::Draw(StatePtr st) {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_nslice + 2);
    glBindVertexArray(0);
}
