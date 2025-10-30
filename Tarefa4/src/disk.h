#include <memory>
class Disk;
using DiskPtr = std::shared_ptr<Disk>;

#ifndef DISK_H
#define DISK_H

#include "shape.h"

class Disk : public Shape {
  unsigned int m_vao;
  int m_nslice;
  float m_radius;  // Novo atributo para o raio
protected:
  Disk(int nslice, float radius);  // Construtor que agora recebe o raio
public:
  static DiskPtr Make(int nslice = 64, float radius = 1.0f);  // Criação do disco com raio
  virtual ~Disk();
  virtual void Draw(StatePtr st);
};

#endif
