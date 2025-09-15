#include "Ticks.h"
#include <vector>
#include <GL/glew.h>
#include <glm/gtc/constants.hpp>
#include <cmath>

TicksPtr Ticks::Make(float outerRadius) { return TicksPtr(new Ticks(outerRadius)); }

Ticks::Ticks(float outerRadius) : m_model(1.0f) { create(outerRadius); }

Ticks::~Ticks() {
  if (m_vaoMin) glDeleteVertexArrays(1, &m_vaoMin);
  if (m_vaoHora) glDeleteVertexArrays(1, &m_vaoHora);
}

void Ticks::SetModel(const glm::mat4& m) { m_model = m; }

void Ticks::create(float outerR) {
  float innerRMin = outerR * 0.94f;
  float innerRHora = outerR * 0.80f;

  std::vector<float> posMin; std::vector<unsigned char> colorMin;
  posMin.reserve(60 * 4); colorMin.reserve(60 * 2 * 3);
  for (int i = 0; i < 60; ++i) {
    float ang = glm::two_pi<float>() * (float)i / 60.0f; float cx = cosf(ang), cy = sinf(ang);
    posMin.push_back(cx * outerR); posMin.push_back(cy * outerR);
    posMin.push_back(cx * innerRMin); posMin.push_back(cy * innerRMin);
    for (int k = 0; k < 2; ++k) { colorMin.push_back(120); colorMin.push_back(120); colorMin.push_back(120); }
  }
  m_minVertexCount = (int)posMin.size() / 2;
  glGenVertexArrays(1, &m_vaoMin); glBindVertexArray(m_vaoMin);
  glGenBuffers(1, &m_vboMinPos); glBindBuffer(GL_ARRAY_BUFFER, m_vboMinPos);
  glBufferData(GL_ARRAY_BUFFER, posMin.size() * sizeof(float), posMin.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0); glEnableVertexAttribArray(0);
  glGenBuffers(1, &m_vboMinColor); glBindBuffer(GL_ARRAY_BUFFER, m_vboMinColor);
  glBufferData(GL_ARRAY_BUFFER, colorMin.size() * sizeof(unsigned char), colorMin.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1,3,GL_UNSIGNED_BYTE,GL_TRUE,0,0); glEnableVertexAttribArray(1);

  std::vector<float> posHora; std::vector<unsigned char> colorHora;
  posHora.reserve(12 * 4); colorHora.reserve(12 * 2 * 3);
  for (int i = 0; i < 12; ++i) {
    float ang = glm::two_pi<float>() * (float)i / 12.0f; float cx = cosf(ang), cy = sinf(ang);
    posHora.push_back(cx * outerR); posHora.push_back(cy * outerR);
    posHora.push_back(cx * innerRHora); posHora.push_back(cy * innerRHora);
    for (int k = 0; k < 2; ++k) { colorHora.push_back(0); colorHora.push_back(0); colorHora.push_back(0); }
  }
  m_horaVertexCount = (int)posHora.size() / 2;
  glGenVertexArrays(1, &m_vaoHora); glBindVertexArray(m_vaoHora);
  glGenBuffers(1, &m_vboHoraPos); glBindBuffer(GL_ARRAY_BUFFER, m_vboHoraPos);
  glBufferData(GL_ARRAY_BUFFER, posHora.size() * sizeof(float), posHora.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0); glEnableVertexAttribArray(0);
  glGenBuffers(1, &m_vboHoraColor); glBindBuffer(GL_ARRAY_BUFFER, m_vboHoraColor);
  glBufferData(GL_ARRAY_BUFFER, colorHora.size() * sizeof(unsigned char), colorHora.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1,3,GL_UNSIGNED_BYTE,GL_TRUE,0,0); glEnableVertexAttribArray(1);
  glBindVertexArray(0);
}

void Ticks::Draw() {
  glLineWidth(1.f); glBindVertexArray(m_vaoMin); glDrawArrays(GL_LINES, 0, m_minVertexCount);
  glLineWidth(3.f); glBindVertexArray(m_vaoHora); glDrawArrays(GL_LINES, 0, m_horaVertexCount);
  glLineWidth(1.f); glBindVertexArray(0);
}
