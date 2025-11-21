#include "patchquad.h"
#include "error.h"

#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

PatchQuadPtr PatchQuad::Make() {
  return PatchQuadPtr(new PatchQuad());
}

PatchQuad::PatchQuad() : m_vao(0), m_count(4) {
  // 4 control points for a quad patch
  float coords[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
     0.5f,  0.5f, 0.0f
  };
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
}

PatchQuad::~PatchQuad() {}

void PatchQuad::Draw(StatePtr) {
  Error::Check("patchquad begin");
  glBindVertexArray(m_vao);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawArrays(GL_PATCHES, 0, m_count);
  Error::Check("patchquad end");
}
