#include "animation.h"

  std::vector<MovementPtr> m_moves;
Animation::Animation (std::initializer_list<MovementPtr> moves)
: m_curr(0), m_moves(moves)
{
}
AnimationPtr Animation::Make (std::initializer_list<MovementPtr> moves)
{
  return AnimationPtr(new Animation(moves));
}
Animation::~Animation ()
{
}

bool Animation::Advance (float dt, bool reverse)  // return true when it's done
{
  // use size_t to avoid signed/unsigned comparison warnings
  size_t idx = reverse ? (m_moves.size()-1)-m_curr : m_curr;
  if (m_moves[idx]->Advance(dt,reverse)) {
    ++m_curr;
    if (m_curr >= m_moves.size()) { // finished cycle
      m_curr = 0;
      return true;
    }
  }
  return false;
}
