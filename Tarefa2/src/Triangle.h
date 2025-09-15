#include <memory>
class Triangle;
using TrianglePtr = std::shared_ptr<Triangle>; 

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "Shader.h"

class Triangle{
public:
  static TrianglePtr Make ();
  void Draw(const Shader& shader);

  void setRotation(float angleDegrees) ; 
  void setScale(float s);
  
  float getRotation();
  
  ~Triangle ();
private:
  Triangle ();

  unsigned int m_vao;
  glm::mat4 m_model;

  void UpdateModelMatrix();

  float m_rotation;
  float m_scale; 

};
#endif