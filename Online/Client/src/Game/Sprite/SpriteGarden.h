#pragma once
#ifndef SPRITEGAREND_H
#define SPRITEGAREND_H

#include <glm/glm.hpp>
#include "Sprite.h"
#include "../SpriteRenderer/SpriteRenderer.h"
#include "../ResourceManager/ResourceManager.h"

class SpriteGarden : public Sprite {
public:
  SpriteGarden(SpriteRenderer* renderer) : Sprite(renderer) {
    this->model = &ResourceManager::LoadModel("resources/objects/Garden.obj", "garden");
    this->Size = glm::vec3(1.2f, 1.0f, 1.2f);
    this->Postion = glm::vec3(0, -4.0f, 0);
  }

  void Render() {
    this->renderer->DrawSprite(*this->model, this->Postion, this->Size);
  }

private:
  Model* model;
};

#endif // !SPRITEGAREND_H
