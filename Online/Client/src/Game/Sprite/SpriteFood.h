#pragma once

#include <glm/glm.hpp>
#include "Sprite.h"
#include "../SpriteRenderer/SpriteRenderer.h"
#include "../ResourceManager/ResourceManager.h"
#include "../Util/Util.h"

class SpriteFood : public Sprite {
public:
  bool First = true;
  SpriteFood(SpriteRenderer* renderer) : Sprite(renderer) {
    // 食物
    ResourceManager::LoadModel("resources/objects/food/Hamburger.obj", "food");
    ResourceManager::LoadModel("resources/objects/food/ball.obj", "ball");
    ResourceManager::LoadModel("resources/objects/food/lemon.obj", "lemon");
    this->foodPos = glm::vec2(0, 0);
    this->foodPosOld = glm::vec2(0, 0);
  }
  void Render() {

  }
  void Render(float currentP) {

    // 渲染食物
    float foodHeight = 0.0f;
    float foodSize = 1.0f;
    if (foodPos != foodPosOld) {
      foodHeight = 20.0f * (1 - UtilTool::scaleValue(currentP));
      foodSize = UtilTool::scaleValue(currentP);
      this->renderFood(currentP, this->styleFoodOld, foodPosOld, 0.0f, 1 - foodSize);
    }
    // 随机动画效果
    if (this->styleFood == 2) {
      this->renderFood(currentP, this->styleFood, foodPos, foodHeight, 1.0f);
    }
    else {
      this->renderFood(currentP, this->styleFood, foodPos, 0.0f, foodSize);
    }
    
  }

  void SetData(glm::vec2 foodPos, int styleFood) {
    this->foodPos = foodPos;
    this->styleFood = styleFood;
  }

  void Update() {
    this->foodPosOld = this->foodPos;
    this->styleFoodOld = this->styleFood;
  }


private:
  glm::vec2 foodPos;
  glm::vec2 foodPosOld;
  int styleFood = 0;
  int styleFoodOld = 0;
  void renderFood(float p, int style, glm::vec2 pos, float height, float size) {
    switch (style) {
    case 1:
      this->renderer->DrawSprite(ResourceManager::GetModel("food"),
        glm::vec3(pos.x, 0.3f + height, pos.y),
        glm::vec3(0.17f * size));
      break;
    case 2:
      this->renderer->DrawSprite(ResourceManager::GetModel("ball"),
        glm::vec3(pos.x, 0.0f + height, pos.y),
        glm::vec3(1 * size));
      break;
    default:
      this->renderer->DrawSprite(ResourceManager::GetModel("lemon"),
        glm::vec3(pos.x, 0.0f + height, pos.y),
        glm::vec3(1.8f * size));
      break;
    }
  }
};

