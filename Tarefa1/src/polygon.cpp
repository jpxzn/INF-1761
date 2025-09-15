#include "Polygon.h"

#include <iostream>

#ifdef _WIN32
#include <glad/glad.h>
#else
#include <GL/glew.h>
#endif

PolygonPtr Polygon::Make ()
{
    return PolygonPtr(new Polygon());
}

Polygon::Polygon ()
{
    float vertices[] = {
        0.0f,  0.2f,
        0.5f, 0.5f,
        0.5f,-0.4f,
       -0.5f,-0.4f,
       -0.5f, 0.5f
    };

    unsigned char colors[] = {
        255, 0, 0,
        255, 255, 0,
        128, 0, 128,
        0, 255, 0,
        0, 255, 255
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vbo[2];
    glGenBuffers(2, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

Polygon::~Polygon ()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_ebo);
}

void Polygon::Draw ()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
}
