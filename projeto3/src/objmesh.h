#ifndef OBJMESH_H
#define OBJMESH_H

#include "shape.h"
#include <memory>
#include <string>
#include <vector>

class ObjMesh;
using ObjMeshPtr = std::shared_ptr<ObjMesh>;

class ObjMesh : public Shape {
  unsigned int m_vao;
  unsigned int m_nind;  // number of indices
  std::vector<unsigned int> m_vbos; // track VBOs for cleanup
protected:
  ObjMesh(const std::string& filename);
public:
  static ObjMeshPtr Make(const std::string& filename);
  virtual ~ObjMesh();
  virtual void Draw(StatePtr st);
};

#endif
