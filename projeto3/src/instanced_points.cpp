#include "instanced_points.h"
#include "error.h"
#include <iostream>

#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

InstancedPointsPtr InstancedPoints::Make(const std::vector<InstanceData>& instances) {
  return InstancedPointsPtr(new InstancedPoints(instances));
}

InstancedPoints::InstancedPoints(const std::vector<InstanceData>& instances)
: m_vao(0), m_vbo(0), m_count(int(instances.size()))
{
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(InstanceData), instances.data(), GL_STATIC_DRAW);
  // layout: pos(3 floats) @ location 4
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, pos));
  glEnableVertexAttribArray(4);
  // scale(3 floats) @ location 5
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, scale));
  glEnableVertexAttribArray(5);
  // typeId (as float) @ location 6
  glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, typeId));
  glEnableVertexAttribArray(6);
  // color(3 floats) @ location 7
  glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
  glEnableVertexAttribArray(7);
}

InstancedPoints::~InstancedPoints() {}

void InstancedPoints::Draw(StatePtr) {
  glBindVertexArray(m_vao);
  glDrawArrays(GL_POINTS, 0, m_count);
}
