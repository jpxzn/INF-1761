#include <memory>
class Cylinder;
using CylinderPtr = std::shared_ptr<Cylinder>;

#ifndef CYLINDER_H
#define CYLINDER_H

#include "shape.h"
#include "disk.h"

class Cylinder : public Shape {
  unsigned int m_vao;
  unsigned int m_nind;
  ShapePtr m_top;  // Tampo superior
  ShapePtr m_bottom;  // Tampo inferior
  float _height;
  protected:
  Cylinder (int nstack, int nslice, float height, float radius);
  public:
  static CylinderPtr Make (int nstack=32, int nslice=64, float height=1.0f, float radius=0.5f);
  virtual ~Cylinder ();
  virtual void Draw (StatePtr st);
};

#endif
