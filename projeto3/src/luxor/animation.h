#include <memory>
class Animation;
using AnimationPtr = std::shared_ptr<Animation>; 

#ifndef ANIMATION_H
#define ANIMATION_H

#include "movement.h"

class Animation {
  size_t m_curr;   // current movement index
  std::vector<MovementPtr> m_moves;
protected:
  Animation (std::initializer_list<MovementPtr> moves);
public:
  static AnimationPtr Make (std::initializer_list<MovementPtr> moves);
  virtual ~Animation ();
  bool Advance (float dt, bool reverse=false);  // return true when its done
};

#endif