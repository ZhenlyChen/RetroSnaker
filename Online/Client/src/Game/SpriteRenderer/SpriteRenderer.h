#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H
#include "../ResourceManager/ResourceManager.h"

class SpriteRenderer {
 public:
  SpriteRenderer(Shader& shader, Shader& fontShader);
  ~SpriteRenderer();

  void DrawSprite(Texture2D& texture, glm::vec3 position,
                  glm::vec3 size = glm::vec3(10, 10, 10), GLfloat rotate = 0.0f,
                  glm::vec3 color = glm::vec3(1.0f));

  void DrawSprite(Model& model, glm::vec3 position,
                  glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f),
                  GLfloat rotate = 0.0f,
                  glm::vec3 value = glm::vec3(0.4, 1, 0.3));

  void RenderText(std::string text, glm::vec2 postion, GLfloat scale = 1.0,
                  glm::vec3 color = glm::vec3(1,1,1));

 private:
  void initRenderData();
  Shader shader;
  Shader fontShader;
  GLuint quadVAO;
};
#endif
