#include <memory>
class InstancedPoints;
using InstancedPointsPtr = std::shared_ptr<InstancedPoints>;

#ifndef INSTANCED_POINTS_H
#define INSTANCED_POINTS_H

#include "shape.h"
#include <vector>
#include <glm/glm.hpp>

struct InstanceData {
  glm::vec3 pos;
  glm::vec3 scale;
  float typeId;
  glm::vec3 color;
};

class InstancedPoints : public Shape {
  unsigned int m_vao;
  unsigned int m_vbo;
  int m_count;
  InstancedPoints(const std::vector<InstanceData>& instances);
public:
  static InstancedPointsPtr Make(const std::vector<InstanceData>& instances);
  virtual ~InstancedPoints();
  virtual void Draw(StatePtr st);
};

#endif
