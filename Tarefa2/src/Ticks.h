#pragma once
#include <memory>
class Ticks;
using TicksPtr = std::shared_ptr<Ticks>;

#ifndef TICKS_H
#define TICKS_H

#include "Shader.h"

class Ticks {
public:
  static TicksPtr Make(float radius, float innerScale = 0.9f);
  ~Ticks();

  // Desenha as 12 marcações (a cada 5 minutos)
  void Draw(const Shader& shader) const;

private:
  Ticks(float radius, float innerScale);
  unsigned int m_vao;
  int m_vertexCount;
};

#endif
