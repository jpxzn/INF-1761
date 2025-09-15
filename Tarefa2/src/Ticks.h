#ifndef TICKS_H
#define TICKS_H

#include <glm/glm.hpp>
#include <memory>

class Ticks;
using TicksPtr = std::shared_ptr<Ticks>;

class Ticks {
public:
  static TicksPtr Make(float outerRadius = 0.6f);
  ~Ticks();
  void Draw();
  void SetModel(const glm::mat4& m);
private:
  Ticks(float outerRadius);
  void create(float outerR);
  glm::mat4 m_model;
  unsigned int m_vaoMin = 0, m_vboMinPos = 0, m_vboMinColor = 0; int m_minVertexCount = 0;
  unsigned int m_vaoHora = 0, m_vboHoraPos = 0, m_vboHoraColor = 0; int m_horaVertexCount = 0;
};

#endif
