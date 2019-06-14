#pragma once
#ifndef SPRITETABLE_H
#define SPRITETABLE_H

#include <glm/glm.hpp>
#include "Sprite.h"
#include "../SpriteRenderer/SpriteRenderer.h"
#include "../ResourceManager/ResourceManager.h"

class SpriteTable : public Sprite {
public:
  SpriteTable(SpriteRenderer* renderer) : Sprite(renderer) {
    this->model = &ResourceManager::LoadModel("resources/objects/wood_table.obj", "table");
    this->Size = glm::vec3(1.0f, 0.5f, 0.72f);
    this->Postion = glm::vec3(0, -3.6f, 0);
  }

  void Render() {
    this->renderer->DrawSprite(*this->model, this->Postion, this->Size);
  }

private:
  Model* model;
};

#endif // !SPRITETABLE_H
