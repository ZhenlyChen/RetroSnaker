#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include <glm/glm.hpp>
#include "../SpriteRenderer/SpriteRenderer.h"

class Sprite {
 public:
  Sprite(SpriteRenderer* renderer) : renderer(renderer) {
    this->Rotation = 0.0f;
    this->Postion = glm::vec3(1.0);
    this->Size = glm::vec3(1.0);
  }
  virtual void Render() = 0;

  glm::vec3 Postion;
  glm::vec3 Size;
  float Rotation;

 protected:
  SpriteRenderer* renderer;
};

#endif  // !SPRITE_H
