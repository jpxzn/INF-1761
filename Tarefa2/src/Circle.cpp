#include "Circle.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

CirclePtr Circle::Make(float radius, int segments)
{
    return CirclePtr(new Circle(radius, segments));
}

Circle::Circle(float radius, int segments)
{
    std::vector<glm::vec2> verts;
    verts.push_back(glm::vec2(0.0f, 0.0f));

    for (int i = 0; i <= segments; i++) {
        float angle = i * glm::two_pi<float>() / segments;
        glm::vec2 v = glm::vec2(glm::cos(angle), glm::sin(angle)) * radius;
        verts.push_back(v);
    }

    m_vertexCount = verts.size();

    std::vector<float> vertices;
    vertices.reserve(m_vertexCount * 2);
    for (auto &v : verts) {
        vertices.push_back(v.x);
        vertices.push_back(v.y);
    }

    std::vector<float> colors(m_vertexCount * 3, 1.0f);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vbo[2];
    glGenBuffers(2, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

Circle::~Circle()
{
    glDeleteVertexArrays(1, &m_vao);
}

void Circle::Draw()
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_vertexCount);
}
