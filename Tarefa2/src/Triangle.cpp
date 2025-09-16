#include "Triangle.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

TrianglePtr Triangle::Make(unsigned char r, unsigned char g, unsigned char b) {
  return TrianglePtr(new Triangle(r, g, b));
}

Triangle::Triangle(unsigned char r, unsigned char g, unsigned char b)
  : m_vao(0)
  , m_colorVBO(0)
  , m_model(1.0f)
  , m_rotation(0.0f)
  , m_scale(1.0f)
{
  float coord[] = {
    -0.03f, 0.0f,
     0.03f, 0.0f,
     0.0f,  0.55f,
  };
  unsigned char color[] = {
    r, g, b,
    r, g, b,
    r, g, b,
  };

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  GLuint id[2];
  glGenBuffers(2, id);

  glBindBuffer(GL_ARRAY_BUFFER, id[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, id[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);
  glEnableVertexAttribArray(1);

  m_colorVBO = id[1];

  glBindVertexArray(0);
}

Triangle::~Triangle() {
  if (m_vao) {
    glDeleteVertexArrays(1, &m_vao);
  }
  if (m_colorVBO) {
    glDeleteBuffers(1, &m_colorVBO);
  }
}

float Triangle::getRotation() {
  return m_rotation;
}

void Triangle::setRotation(float angleDegrees) {
  m_rotation = angleDegrees;
  UpdateModelMatrix();
}

void Triangle::setScale(float s) {
  m_scale = s;
  UpdateModelMatrix();
}

void Triangle::setColor(unsigned char r, unsigned char g, unsigned char b) {
  if (!m_colorVBO) return;
  unsigned char color[9] = {
    r,g,b,
    r,g,b,
    r,g,b
  };
  glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Triangle::UpdateModelMatrix() {
  m_model = glm::mat4(1.0f);
  m_model = glm::scale(m_model, glm::vec3(m_scale, m_scale, 1.0f));
  m_model = glm::rotate(m_model, glm::radians(m_rotation), glm::vec3(0,0,1));
}

void Triangle::Draw(const Shader& shader) {
  shader.UseProgram();
  shader.SetUniform("uModel", m_model);
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}