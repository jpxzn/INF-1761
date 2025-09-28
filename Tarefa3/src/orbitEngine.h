#pragma once

#include "engine.h"
#include "transform.h"

class OrbitEngine;
using OrbitEnginePtr = std::shared_ptr<OrbitEngine>;

class OrbitEngine : public Engine {
    float m_deg_per_sec;
    TransformPtr m_trf;
protected:
    OrbitEngine(TransformPtr trf, float deg_per_sec);
public:
    static OrbitEnginePtr Make(TransformPtr trf, float deg_per_sec);
    void Update(float dt) override;
};
