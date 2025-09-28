#include "orbitEngine.h"
#include <cmath>

OrbitEngine::OrbitEngine(TransformPtr trf, float deg_per_sec)
: m_trf(trf), m_deg_per_sec(deg_per_sec) {}

OrbitEnginePtr OrbitEngine::Make(TransformPtr trf, float deg_per_sec) {
    return OrbitEnginePtr(new OrbitEngine(trf, deg_per_sec));
}

void OrbitEngine::Update(float dt) {
    if (m_trf) {
        m_trf->Rotate(m_deg_per_sec * dt, 0, 0, 1);
    }
}
