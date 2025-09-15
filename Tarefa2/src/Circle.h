#include <memory>
class Circle;
using CirclePtr = std::shared_ptr<Circle>; 

#ifndef CIRCLE_H
#define CIRCLE_H

class Circle{
  unsigned int m_vao;
  int m_vertexCount;
protected:
  Circle (float radius, int segments);
public:
  static CirclePtr Make (float radius, int segments);
  ~Circle ();
  void Draw ();
};
#endif