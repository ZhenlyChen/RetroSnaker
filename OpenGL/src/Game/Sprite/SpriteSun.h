#pragma once
#ifndef SPRITESUN_H
#define SPRITESUN_H

#include <glm/glm.hpp>
#include "Sprite.h"
#include "../SpriteRenderer/SpriteRenderer.h"
#include "../ResourceManager/ResourceManager.h"

class SpriteSun : public Sprite {
public:
  SpriteSun(SpriteRenderer* renderer) : Sprite(renderer) {
    this->model = &ResourceManager::LoadModel("resources/objects/sun.obj", "sun");
    this->Size = glm::vec3(0.3f, 0.3f, 0.3f);
    this->Postion = glm::vec3(0, 5, 0);
  }

  void Render() {
    this->renderer->DrawSprite(*this->model, this->Postion, this->Size);
  }

private:
  Model* model;
};

#endif // !SPRITESUN_H
