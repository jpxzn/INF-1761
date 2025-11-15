#include <memory>
class LuxorEngine;
using LuxorEnginePtr = std::shared_ptr<LuxorEngine>; 

#ifndef LUXOR_ENGINE_H
#define LUXOR_ENGINE_H

#include "engine.h"
#include "animation.h"
#include <glm/glm.hpp>

class LuxorEngine : public Engine {
  bool m_reverse;
  float m_head_angle;
  std::string m_status;
  AnimationPtr m_curr_anim;
  AnimationPtr m_stand_down_anim;
  AnimationPtr m_jump_forward_anim;
  AnimationPtr m_flip_anim;
  TransformPtr m_trf_all;
  TransformPtr m_trf_base;
  TransformPtr m_trf_haste1;
  TransformPtr m_trf_haste2;
  TransformPtr m_trf_haste3;
  TransformPtr m_trf_cupula;
  TransformPtr m_trf_lampada;
  glm::mat4 m_trf_all_orig;
  glm::mat4 m_trf_base_orig;
  glm::mat4 m_trf_haste1_orig;
  glm::mat4 m_trf_haste2_orig;
  glm::mat4 m_trf_haste3_orig;
  glm::mat4 m_trf_cupula_orig;
  glm::mat4 m_trf_lampada_orig;
  protected:
  LuxorEngine (TransformPtr trf_all,
               TransformPtr trf_base,
               TransformPtr trf_haste1,
               TransformPtr trf_haste2,
               TransformPtr trf_haste3,
               TransformPtr trf_cupula,
               TransformPtr trf_lampada
              );
  public:
  static LuxorEnginePtr Make (TransformPtr trf_all,
                              TransformPtr trf_base,
                              TransformPtr trf_haste1,
                              TransformPtr trf_haste2,
                              TransformPtr trf_haste3,
                              TransformPtr trf_cupula,
                              TransformPtr trf_lampada
                              );
  virtual ~LuxorEngine ();
  bool StandUp ();
  bool StandDown ();
  bool JumpForward ();
  bool flip ();
  bool JumpBackward ();
  void TurnHead (float angle);
  virtual void Update (float dt);
private:
  void CreateStandDownAnimation ();
  void CreateJumpForwardAnimation ();
  void CreateflipAnimation ();
};

#endif