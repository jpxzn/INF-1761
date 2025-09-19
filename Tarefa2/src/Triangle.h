#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

class Triangle;
using TrianglePtr = std::shared_ptr<Triangle>;

class Triangle {
public:
  static TrianglePtr Make(unsigned char r = 255, unsigned char g = 0, unsigned char b = 0);

  void Draw(const Shader& shader);

  void setRotation(float angleDegrees);
  void setScale(float s);
  float getRotation();

  void setColor(unsigned char r, unsigned char g, unsigned char b);

  ~Triangle();

private:
  Triangle(unsigned char r, unsigned char g, unsigned char b);

  void UpdateModelMatrix();

  unsigned int m_vao;
  unsigned int m_colorVBO;
  glm::mat4 m_model;
  float m_rotation;
  float m_scale;
};

#endif