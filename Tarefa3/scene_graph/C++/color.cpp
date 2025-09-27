#include "color.h"
#include "shader.h"
#include "state.h"

#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

ColorPtr Color::Make (float r, float g, float b, float a)
{
  return ColorPtr(new Color(r,g,b,a));
}

Color::Color (float r, float g, float b, float a)
: m_color(r,g,b,a)
{
}
Color::~Color ()
{
}
void Color::Load (StatePtr st)
{
  ShaderPtr shd = st->GetShader();
  shd->SetUniform("color",m_color);
}
