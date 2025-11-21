#include <memory>
class PatchQuad;
using PatchQuadPtr = std::shared_ptr<PatchQuad>;

#ifndef PATCHQUAD_H
#define PATCHQUAD_H

#include "shape.h"

class PatchQuad : public Shape {
  unsigned int m_vao;
  int m_count;
protected:
  PatchQuad();
public:
  static PatchQuadPtr Make();
  virtual ~PatchQuad();
  virtual void Draw(StatePtr st);
};
#endif
