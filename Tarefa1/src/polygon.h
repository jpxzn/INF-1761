#include <memory>
class Polygon;
using PolygonPtr = std::shared_ptr<Polygon>; 

#ifndef POLYGON_H
#define POLYGON_H

#include "shape.h"

class Polygon : public Shape {
  unsigned int m_vao;
  unsigned int m_ebo;
protected:
  Polygon ();
public:
  static PolygonPtr Make ();
  virtual ~Polygon ();
  virtual void Draw ();
};
#endif