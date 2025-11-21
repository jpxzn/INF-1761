#include "luxorengine.h"
#include "linearinterpolator.h"
#include "cubicinterpolator.h"
#include <iostream>

LuxorEngine::LuxorEngine (
               TransformPtr trf_all,
               TransformPtr trf_base,
               TransformPtr trf_haste1,
               TransformPtr trf_haste2,
               TransformPtr trf_haste3,
               TransformPtr trf_cupula,
               TransformPtr trf_lampada
              )
: m_reverse(false), 
  m_head_angle(0.0f),
  m_status("up"), 
  m_curr_anim(),
  m_trf_all(trf_all),
  m_trf_base(trf_base),
  m_trf_haste1(trf_haste1),
  m_trf_haste2(trf_haste2),
  m_trf_haste3(trf_haste3),
  m_trf_cupula(trf_cupula),
  m_trf_lampada(trf_lampada)
{
  m_trf_all_orig = m_trf_all->GetMatrix();
  CreateStandDownAnimation();
  CreateJumpForwardAnimation();
  CreateJumpLeftAnimation();
  CreateJumpFrontAnimation();
  CreateJumpBackAnimation();
  CreateflipAnimation();
}

LuxorEnginePtr LuxorEngine::Make (
               TransformPtr trf_all,
               TransformPtr trf_base,
               TransformPtr trf_haste1,
               TransformPtr trf_haste2,
               TransformPtr trf_haste3,
               TransformPtr trf_cupula,
               TransformPtr trf_lampada
              )
{
  return LuxorEnginePtr(new LuxorEngine(trf_all,
                                        trf_base,
                                        trf_haste1,
                                        trf_haste2,
                                        trf_haste3,
                                        trf_cupula,
                                        trf_lampada
                                       )
                        );
}

LuxorEngine::~LuxorEngine () 
{
}

void LuxorEngine::CreateStandDownAnimation ()
{
  MovementPtr move = Movement::Make(0.5f);
  move->AddRotation(m_trf_haste1,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(-30.0f,0.0f,0.0f)
                                            )
                   );
  move->AddRotation(m_trf_haste2,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(120.0f,0.0f,0.0f)
                                            )
                   );
  move->AddRotation(m_trf_haste3,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(-120.0f,0.0f,0.0f)
                                            )
                   );
  move->AddRotation(m_trf_cupula,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(30.0f,0.0f,0.0f)
                                            )
                   );
  m_stand_down_anim = Animation::Make({move});
}

void LuxorEngine::CreateJumpForwardAnimation ()
{
  // Right jump (use X+ lateral translation)
  // Key frames (x rotations in degrees):
  // base:   [  0,   0, -30,   0,   0]
  // haste1: [-30, -40,  10, -60, -30]
  // haste2: [120, 150,  50, 160, 120]
  // haste3: [-120,145, -50, 165,-120]
  // cupula: [ 30,  60,  65,  60,  30]
  // lampada: all zeros
  // Translations (all) — lateral X+: (0,0,0) (0,0,0) (30,30,0) (90,0,0) (90,0,0)

  // frame1 -> frame2
  MovementPtr move1 = Movement::Make(0.3f);
  move1->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(0.0f), glm::vec3(0.0f)));
  move1->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f),
                                              glm::vec3(-40.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(120.0f,0.0f,0.0f),
                                              glm::vec3(150.0f,0.0f,0.0f)));
  // keep continuity for haste3 (avoid wrapping across +/−180 abruptly)
  move1->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-120.0f,0.0f,0.0f),
                                              glm::vec3(-145.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(30.0f,0.0f,0.0f),
                                              glm::vec3(60.0f,0.0f,0.0f)));

  // frame2 -> frame3 (jump up & right)
  MovementPtr move2 = Movement::Make(0.5f);
  move2->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                                glm::vec3(15.0f,15.0f,0.0f),
                                                glm::vec3(25.0f,25.0f,0.0f),
                                                glm::vec3(30.0f,30.0f,0.0f)));
  move2->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                              glm::vec3(-30.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(-40.0f,0.0f,0.0f),
                                              glm::vec3(10.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(150.0f,0.0f,0.0f),
                                              glm::vec3(50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-145.0f,0.0f,0.0f),
                                              glm::vec3(-50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),
                                              glm::vec3(65.0f,0.0f,0.0f)));

  // frame3 -> frame4 (go right/down)
  MovementPtr move3 = Movement::Make(0.5f);
  move3->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(30.0f,30.0f,0.0f),
                                                glm::vec3(70.0f,15.0f,0.0f),
                                                glm::vec3(85.0f,5.0f,0.0f),
                                                glm::vec3(90.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f),
                                              glm::vec3(0.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(10.0f,0.0f,0.0f),
                                              glm::vec3(-60.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(50.0f,0.0f,0.0f),
                                              glm::vec3(160.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-50.0f,0.0f,0.0f),
                                              glm::vec3(-165.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(65.0f,0.0f,0.0f),
                                              glm::vec3(60.0f,0.0f,0.0f)));

  // frame4 -> frame5 (settle back to down pose in front)
  MovementPtr move4 = Movement::Make(0.3f);
  move4->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                              glm::vec3(0.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(-60.0f,0.0f,0.0f),
                                              glm::vec3(-30.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(160.0f,0.0f,0.0f),
                                              glm::vec3(120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-165.0f,0.0f,0.0f),
                                              glm::vec3(-120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),
                                              glm::vec3(30.0f,0.0f,0.0f)));

  m_jump_forward_anim = Animation::Make({move1,move2,move3,move4});
}

void LuxorEngine::CreateJumpLeftAnimation ()
{
  // Mirror of right jump along X (negative X)
  MovementPtr move1 = Movement::Make(0.3f);
  move1->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(0.0f), glm::vec3(0.0f)));
  move1->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f),
                                              glm::vec3(-40.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(120.0f,0.0f,0.0f),
                                              glm::vec3(150.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-120.0f,0.0f,0.0f),
                                              glm::vec3(-145.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(30.0f,0.0f,0.0f),
                                              glm::vec3(60.0f,0.0f,0.0f)));

  MovementPtr move2 = Movement::Make(0.5f);
  move2->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                                glm::vec3(-15.0f,15.0f,0.0f),
                                                glm::vec3(-25.0f,25.0f,0.0f),
                                                glm::vec3(-30.0f,30.0f,0.0f)));
  move2->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                              glm::vec3(-30.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(-40.0f,0.0f,0.0f),
                                              glm::vec3(10.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(150.0f,0.0f,0.0f),
                                              glm::vec3(50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-145.0f,0.0f,0.0f),
                                              glm::vec3(-50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),
                                              glm::vec3(65.0f,0.0f,0.0f)));

  MovementPtr move3 = Movement::Make(0.5f);
  move3->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(-30.0f,30.0f,0.0f),
                                                glm::vec3(-70.0f,15.0f,0.0f),
                                                glm::vec3(-85.0f,5.0f,0.0f),
                                                glm::vec3(-90.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f),
                                              glm::vec3(0.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(10.0f,0.0f,0.0f),
                                              glm::vec3(-60.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(50.0f,0.0f,0.0f),
                                              glm::vec3(160.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-50.0f,0.0f,0.0f),
                                              glm::vec3(-165.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(65.0f,0.0f,0.0f),
                                              glm::vec3(60.0f,0.0f,0.0f)));

  MovementPtr move4 = Movement::Make(0.3f);
  move4->AddRotation(m_trf_base,
                     LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                              glm::vec3(0.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste1,
                     LinearInterpolator::Make(glm::vec3(-60.0f,0.0f,0.0f),
                                              glm::vec3(-30.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste2,
                     LinearInterpolator::Make(glm::vec3(160.0f,0.0f,0.0f),
                                              glm::vec3(120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste3,
                     LinearInterpolator::Make(glm::vec3(-165.0f,0.0f,0.0f),
                                              glm::vec3(-120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_cupula,
                     LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),
                                              glm::vec3(30.0f,0.0f,0.0f)));

  m_jump_left_anim = Animation::Make({move1,move2,move3,move4});
}

void LuxorEngine::CreateJumpFrontAnimation ()
{
  // Forward jump (Z+)
  MovementPtr move1 = Movement::Make(0.3f);
  move1->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(0.0f), glm::vec3(0.0f)));
  move1->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f), glm::vec3(-40.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(120.0f,0.0f,0.0f), glm::vec3(150.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-120.0f,0.0f,0.0f), glm::vec3(-145.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(30.0f,0.0f,0.0f),   glm::vec3(60.0f,0.0f,0.0f)));

  MovementPtr move2 = Movement::Make(0.5f);
  move2->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                                glm::vec3(0.0f,15.0f,25.0f),
                                                glm::vec3(0.0f,25.0f,40.0f),
                                                glm::vec3(0.0f,30.0f,50.0f)));
  move2->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),   glm::vec3(-30.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(-40.0f,0.0f,0.0f),  glm::vec3(10.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(150.0f,0.0f,0.0f),  glm::vec3(50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-145.0f,0.0f,0.0f), glm::vec3(-50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),   glm::vec3(65.0f,0.0f,0.0f)));

  MovementPtr move3 = Movement::Make(0.5f);
  move3->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(0.0f,30.0f,50.0f),
                                                glm::vec3(0.0f,15.0f,70.0f),
                                                glm::vec3(0.0f,5.0f,85.0f),
                                                glm::vec3(0.0f,0.0f,90.0f)));
  move3->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f),  glm::vec3(0.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(10.0f,0.0f,0.0f),   glm::vec3(-60.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(50.0f,0.0f,0.0f),   glm::vec3(160.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-50.0f,0.0f,0.0f),  glm::vec3(-165.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(65.0f,0.0f,0.0f),   glm::vec3(60.0f,0.0f,0.0f)));

  MovementPtr move4 = Movement::Make(0.3f);
  move4->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(0.0f), glm::vec3(0.0f)));
  move4->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(-60.0f,0.0f,0.0f), glm::vec3(-30.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(160.0f,0.0f,0.0f), glm::vec3(120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-165.0f,0.0f,0.0f),glm::vec3(-120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),  glm::vec3(30.0f,0.0f,0.0f)));

  m_jump_front_anim = Animation::Make({move1,move2,move3,move4});
}

void LuxorEngine::CreateJumpBackAnimation ()
{
  // Backward jump (Z-)
  MovementPtr move1 = Movement::Make(0.3f);
  move1->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(0.0f), glm::vec3(0.0f)));
  move1->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f), glm::vec3(-40.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(120.0f,0.0f,0.0f), glm::vec3(150.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-120.0f,0.0f,0.0f), glm::vec3(-145.0f,0.0f,0.0f)));
  move1->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(30.0f,0.0f,0.0f),   glm::vec3(60.0f,0.0f,0.0f)));

  MovementPtr move2 = Movement::Make(0.5f);
  move2->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                                glm::vec3(0.0f,15.0f,-25.0f),
                                                glm::vec3(0.0f,25.0f,-40.0f),
                                                glm::vec3(0.0f,30.0f,-50.0f)));
  move2->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),   glm::vec3(-30.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(-40.0f,0.0f,0.0f),  glm::vec3(10.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(150.0f,0.0f,0.0f),  glm::vec3(50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-145.0f,0.0f,0.0f), glm::vec3(-50.0f,0.0f,0.0f)));
  move2->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),   glm::vec3(65.0f,0.0f,0.0f)));

  MovementPtr move3 = Movement::Make(0.5f);
  move3->AddTranslation(m_trf_all,
                        CubicInterpolator::Make(glm::vec3(0.0f,30.0f,-50.0f),
                                                glm::vec3(0.0f,15.0f,-70.0f),
                                                glm::vec3(0.0f,5.0f,-85.0f),
                                                glm::vec3(0.0f,0.0f,-90.0f)));
  move3->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(-30.0f,0.0f,0.0f),  glm::vec3(0.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(10.0f,0.0f,0.0f),   glm::vec3(-60.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(50.0f,0.0f,0.0f),   glm::vec3(160.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-50.0f,0.0f,0.0f),  glm::vec3(-165.0f,0.0f,0.0f)));
  move3->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(65.0f,0.0f,0.0f),   glm::vec3(60.0f,0.0f,0.0f)));

  MovementPtr move4 = Movement::Make(0.3f);
  move4->AddRotation(m_trf_base,   LinearInterpolator::Make(glm::vec3(0.0f), glm::vec3(0.0f)));
  move4->AddRotation(m_trf_haste1, LinearInterpolator::Make(glm::vec3(-60.0f,0.0f,0.0f), glm::vec3(-30.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste2, LinearInterpolator::Make(glm::vec3(160.0f,0.0f,0.0f), glm::vec3(120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_haste3, LinearInterpolator::Make(glm::vec3(-165.0f,0.0f,0.0f),glm::vec3(-120.0f,0.0f,0.0f)));
  move4->AddRotation(m_trf_cupula, LinearInterpolator::Make(glm::vec3(60.0f,0.0f,0.0f),  glm::vec3(30.0f,0.0f,0.0f)));

  m_jump_back_anim = Animation::Make({move1,move2,move3,move4});
}

void LuxorEngine::CreateflipAnimation ()
{
  MovementPtr move1 = Movement::Make(0.35f);

  move1->AddRotation(m_trf_haste1,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(-30.0f,0.0f,0.0f)
                                            )
                   );
  move1->AddRotation(m_trf_haste2,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(120.0f,0.0f,0.0f)
                                            )
                   );
  move1->AddRotation(m_trf_haste3,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(-120.0f,0.0f,0.0f)
                                            )
                   );
  move1->AddRotation(m_trf_cupula,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(30.0f,0.0f,0.0f)
                                            )
                   );

  //
  // MOVIMENTO 2 — Flip completo + Stretch + Follow-through
  //
  MovementPtr move2 = Movement::Make(0.65f);
  
  move2->AddTranslation(
    m_trf_all,
    CubicInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                            glm::vec3(0.0f,20.0f,0.0f),
                            glm::vec3(0.0f,45.0f,0.0f),
                            glm::vec3(0.0f,70.0f,0.0f))
  );

  move2->AddRotation(m_trf_haste1,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(30.0f,0.0f,0.0f))
  );

  move2->AddRotation(m_trf_haste2,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(-120.0f,0.0f,0.0f))
  );

  move2->AddRotation(m_trf_haste3,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(120.0f,0.0f,0.0f))
  );

  move2->AddRotation(m_trf_cupula,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(-30.0f,0.0f,0.0f))
  );

  MovementPtr move3 = Movement::Make(0.5f);
  // rotação principal do flip

  move3->AddRotation(
    m_trf_all,
    LinearInterpolator::Make(glm::vec3(0.0f),
                             glm::vec3(-180.0f, 0.0f, 0.0f))
  );

  move3->AddRotation(m_trf_haste1,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(-30.0f,0.0f,0.0f)
                                            )
                   );
  move3->AddRotation(m_trf_haste2,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(120.0f,0.0f,0.0f)
                                            )
                   );
  move3->AddRotation(m_trf_haste3,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(-120.0f,0.0f,0.0f)
                                            )
                   );
  move3->AddRotation(m_trf_cupula,
                    LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                                             glm::vec3(30.0f,0.0f,0.0f)
                                            )
                   );
  

  
  MovementPtr move4 = Movement::Make(0.5f);
  // rotação principal do flip
  move4->AddRotation(
    m_trf_all,
    LinearInterpolator::Make(glm::vec3(0.0f),
                             glm::vec3(-180.0f, 0.0f, 0.0f))
  );

  MovementPtr move5 = Movement::Make(0.5f);

  move5->AddTranslation(m_trf_all,
  CubicInterpolator::Make(glm::vec3(0.0f,60.0f,0.0f),
                          glm::vec3(0.0f,30.0f,0.0f),
                          glm::vec3(0.0f,10.0f,0.0f),
                          glm::vec3(0.0f,0.0f,0.0f)
                          )
  );
  move5->AddRotation(m_trf_haste1,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(30.0f,0.0f,0.0f))
  );

  move5->AddRotation(m_trf_haste2,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(-120.0f,0.0f,0.0f))
  );

  move5->AddRotation(m_trf_haste3,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(120.0f,0.0f,0.0f))
  );

  move5->AddRotation(m_trf_cupula,
      LinearInterpolator::Make(glm::vec3(0.0f,0.0f,0.0f),
                              glm::vec3(-30.0f,0.0f,0.0f))
  );
  m_flip_anim = Animation::Make({move1, move2, move3, move4, move5});
}


bool LuxorEngine::flip ()
{
  if (m_trf_all) m_trf_all_orig = m_trf_all->GetMatrix();
  if (m_trf_base) m_trf_base_orig = m_trf_base->GetMatrix();
  if (m_trf_haste1) m_trf_haste1_orig = m_trf_haste1->GetMatrix();
  if (m_trf_haste2) m_trf_haste2_orig = m_trf_haste2->GetMatrix();
  if (m_trf_haste3) m_trf_haste3_orig = m_trf_haste3->GetMatrix();
  if (m_trf_cupula) m_trf_cupula_orig = m_trf_cupula->GetMatrix();
  if (m_trf_lampada) m_trf_lampada_orig = m_trf_lampada->GetMatrix();
  m_curr_anim = m_flip_anim;
  std::cout << "LuxorEngine: flip started\n";
  m_reverse = false;
  m_status = "down";
  return true;
}

bool LuxorEngine::StandUp ()
{
  if (m_curr_anim || m_status != "down")
    return false;
  m_curr_anim = m_stand_down_anim;
  m_reverse = true;
  m_status = "up";
  return true;
}

bool LuxorEngine::StandDown ()
{
  if (m_curr_anim || m_status != "up")
    return false;
  m_curr_anim = m_stand_down_anim;
  m_reverse = false;
  m_status = "down";
  return true;
}

bool LuxorEngine::JumpRight ()
{
  if (m_curr_anim || m_status != "down")
    return false;
  m_curr_anim = m_jump_forward_anim; // right jump
  m_reverse = false;
  m_status = "down";
  return true;
}

bool LuxorEngine::JumpLeft ()
{
  if (m_curr_anim || m_status != "down")
    return false;
  m_curr_anim = m_jump_left_anim; // left jump
  m_reverse = false;
  m_status = "down";
  return true;
}

bool LuxorEngine::JumpForward ()
{
  if (m_curr_anim || m_status != "down")
    return false;
  m_curr_anim = m_jump_front_anim; // forward +Z
  m_reverse = false;
  m_status = "down";
  return true;
}

bool LuxorEngine::JumpBackward ()
{
  if (m_curr_anim || m_status != "down")
    return false;
  m_curr_anim = m_jump_back_anim; // backward -Z
  m_reverse = false;
  m_status = "down";
  return true;
}

void LuxorEngine::TurnHead (float angle)
{
  m_trf_cupula->Rotate(angle,0.0f,1.0f,0.0f);
  m_head_angle += angle;
}

void LuxorEngine::Update (float dt)
{
  if (m_curr_anim) {
    AnimationPtr running = m_curr_anim;
    if (running->Advance(dt,m_reverse)) {
  if (running == m_flip_anim) {
        if (m_trf_all) { m_trf_all->LoadIdentity(); m_trf_all->MultMatrix(m_trf_all_orig); }
        if (m_trf_base) { m_trf_base->LoadIdentity(); m_trf_base->MultMatrix(m_trf_base_orig); }
        if (m_trf_haste1) { m_trf_haste1->LoadIdentity(); m_trf_haste1->MultMatrix(m_trf_haste1_orig); }
        if (m_trf_haste2) { m_trf_haste2->LoadIdentity(); m_trf_haste2->MultMatrix(m_trf_haste2_orig); }
        if (m_trf_haste3) { m_trf_haste3->LoadIdentity(); m_trf_haste3->MultMatrix(m_trf_haste3_orig); }
        if (m_trf_cupula) { m_trf_cupula->LoadIdentity(); m_trf_cupula->MultMatrix(m_trf_cupula_orig); }
        if (m_trf_lampada) { m_trf_lampada->LoadIdentity(); m_trf_lampada->MultMatrix(m_trf_lampada_orig); }
        std::cout << "LuxorEngine: flip finished\n";
      }
      m_curr_anim = nullptr;
    }
  }
}