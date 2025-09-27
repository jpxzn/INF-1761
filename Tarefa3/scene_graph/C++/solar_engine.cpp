#include "solar_engine.h"

#ifdef _WIN32
#include <glad/gl.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

SolarEngine::SolarEngine(TransformPtr earthOrbit, TransformPtr earthSpin, TransformPtr moonOrbit)
: m_earthOrbit(earthOrbit), m_earthSpin(earthSpin), m_moonOrbit(moonOrbit)
{}

void SolarEngine::Update(float dt) {
  if (m_earthOrbit) m_earthOrbit->Rotate(20.0f * dt, 0,0,1);
  if (m_earthSpin)  m_earthSpin->Rotate(90.0f * dt, 0,0,1);
  if (m_moonOrbit)  m_moonOrbit->Rotate(120.0f * dt, 0,0,1);
}
