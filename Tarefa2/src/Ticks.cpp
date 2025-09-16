#include "Ticks.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

TicksPtr Ticks::Make(float radius, float innerScale) {
  return TicksPtr(new Ticks(radius, innerScale));
}

Ticks::Ticks(float radius, float innerScale) : m_vao(0), m_vertexCount(0) {
  std::vector<glm::vec2> verts;
  std::vector<glm::vec3> colors;

  const int tickCount = 60;
  const float innerR = radius * innerScale; // Tamanho maior
  const float outerR = radius; // Raio maior

  for (int i = 0; i < tickCount; ++i) {
    float ang = (glm::two_pi<float>() / tickCount) * i;
    glm::vec2 inner(innerR * std::cos(ang), innerR * std::sin(ang));
    glm::vec2 outer(outerR * std::cos(ang), outerR * std::sin(ang));

    if (i % 5 == 0) {
      // Marca maior
      verts.push_back(inner);
      verts.push_back(outer);
      colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
      colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    } else {
      // Marca menor
      float innerSmallR = outerR - (outerR - innerR) * 0.5f;
      glm::vec2 innerSmall(innerSmallR * std::cos(ang), innerSmallR * std::sin(ang));
      glm::vec2 outerSmall(outerR * std::cos(ang), outerR * std::sin(ang));

      verts.push_back(innerSmall);
      verts.push_back(outerSmall);
      colors.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
      colors.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
    }
  }

  m_vertexCount = static_cast<int>(verts.size());

  unsigned int vboPos = 0, vboCol = 0;
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  glGenBuffers(1, &vboPos);
  glBindBuffer(GL_ARRAY_BUFFER, vboPos);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec2), verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &vboCol);
  glBindBuffer(GL_ARRAY_BUFFER, vboCol);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  glDeleteBuffers(1, &vboPos);
  glDeleteBuffers(1, &vboCol);
}

Ticks::~Ticks() {
  if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void Ticks::Draw(const Shader& shader) const {
  shader.UseProgram();
  shader.SetUniform("uModel", glm::mat4(1.0f));

  glBindVertexArray(m_vao);
  glDrawArrays(GL_LINES, 0, m_vertexCount);
  glBindVertexArray(0);
}
