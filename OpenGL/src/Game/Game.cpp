#include "Game.h"

const int MAX_X = 5;
const int MAX_Y = 6;

// 获取当前旋转位置
glm::vec4 getNowEyePos() {
  glm::vec4 eyePos = glm::vec4(1.0f);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0, 12, 0));
  model = glm::rotate(model, (float)glfwGetTime() / 3, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::translate(model, glm::vec3(36, 0, 0));
  eyePos = model * eyePos;
  return eyePos;
}

GameApp::GameApp() {
  srand((int)time(0));
  this->SetWindowStyle(this->WindowTitle, this->WindowHeight, this->WindowWidth);
  int size = (MAX_X * 2 + 1) * (MAX_Y * 2 + 1);

}

void GameApp::Init(GLFWwindow* window) {
  this->window = window;
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // 加载字体
  ResourceManager::InitFont("resources/fonts/RAVIE.TTF");
  // 加载着色器
  ResourceManager::LoadShader("Game/glsl/object.vs.glsl", 
                              "Game/glsl/object.fs.glsl", nullptr, "object").Use().
    SetVector3f("light.color", glm::vec3(1, 0.88, 0.88));
  ResourceManager::LoadShader("Game/glsl/font.vs.glsl", 
                              "Game/glsl/font.fs.glsl", nullptr, "font").Use().
    SetMatrix4("projection", glm::ortho(0.0f, (float)this->WindowWidth, 0.0f, (float)this->WindowHeight));
  // 初始化渲染管理器
  Renderer = new SpriteRenderer(ResourceManager::GetShader("object"), 
                                ResourceManager::GetShader("font"));
  // 初始化游戏精灵对象
  this->spriteSnake = new SpriteSnake(this->Renderer, MAX_X, MAX_Y);
  this->spriteSun = new SpriteSun(this->Renderer);
  this->spriteGarden = new SpriteGarden(this->Renderer);
  this->spriteTable = new SpriteTable(this->Renderer);
  // 初始化背景颜色
  this->clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  // 初始化摄像机
  this->camera = new Camera(window);
  this->camera->SetLookPostion(glm::vec3(20, 20, 30));
  // 初始化状态
  this->state = GAME_MENU;
  // 重置状态
  this->resetGame();
}

void GameApp::UpdateSize(int w, int h) {
  this->WindowHeight = h;
  this->WindowWidth = w;
  ResourceManager::GetShader("font").Use().
    SetMatrix4("projection", glm::ortho(0.0f, (float)this->WindowWidth, 0.0f, (float)this->WindowHeight));
}

void GameApp::Update() {
  this->camera->Update();
  this->processInput();
  // 渲染场景
  this->render();
  // 每n帧更新一次
  if (frame > updateFrameCount) {
    this->updateGame();
    frame = 0;
  }
  frame++;
}

void GameApp::Clear() {
  ResourceManager::Clear();
}

void GameApp::render() {
  // 背景颜色
  glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // 计算视图
  Shader objectShader = ResourceManager::GetShader("object").Use();
  if (this->state == GAME_MENU) {
    this->camera->SetLookPostion(getNowEyePos());
  }
  objectShader.SetVector3f("viewPos", camera->Position);
  objectShader.SetVector3f("light.position", this->spriteSun->Postion);
  objectShader.SetMatrix4("projection", 
    glm::perspective((float)glm::radians(camera->Zoom), this->WindowWidth / (float)this->WindowHeight, 0.1f, 100.0f));
  objectShader.SetMatrix4("view", camera->GetViewMatrix());

  // 渲染精灵
  this->spriteSnake->Render(frame / (float)updateFrameCount);
  this->spriteSun->Render();
  this->spriteGarden->Render();
  this->spriteTable->Render();

  // 菜单场景
  if (this->state == GAME_MENU) {
    Renderer->RenderText("3D Happy Snake", glm::vec2(100, this->WindowHeight - 100), 1.2);
    Renderer->RenderText("Press ENTER to start game", glm::vec2(100, this->WindowHeight - 150), 0.5);
    if (this->spriteSnake->GetLenght() > 1 && this->first == false) {
      Renderer->RenderText("Your score: " + std::to_string(this->spriteSnake->GetLenght() - 2), glm::vec2(100, this->WindowHeight - 200), 0.8);
    }
  }

  // 加载中动画
  if (this->state == GAME_LOAD) {
    float totalStep = 120.0f;
    float current =loadStep / totalStep;
    glm::vec3 targetPos = glm::vec3(0, 10, 17);
    this->camera->TransitionTo(targetPos, loadStep / totalStep);

    this->spriteSun->Postion.y = 5.0f + (20.0f * current);
    loadStep++;
    glm::vec2 fontPos = glm::vec2(100, this->WindowHeight - 100);
    if (loadStep < 30) {
      Renderer->RenderText("3", fontPos);
    }
    else if (loadStep < 60) {
      Renderer->RenderText("2", fontPos);
    }
    else if (loadStep < 90) {
      Renderer->RenderText("1", fontPos);
    }
    else if (loadStep < 100) {
      Renderer->RenderText("GO", fontPos);
    }
    if (this->loadStep > totalStep) {
      this->state = GAME_ACTIVE;
      this->loadStep = 0;
    }
  }

  // 游戏开始
  if (this->state == GAME_ACTIVE) {
    Renderer->RenderText(
      "Your score: " + std::to_string(this->spriteSnake->GetLenght() - 2),
      glm::vec2(40, this->WindowHeight - 60));
  }

  // 游戏暂停
  if (this->state == GAME_PAUSE) {
    Renderer->RenderText("Pause", glm::vec2(40, this->WindowHeight - 60));
  }

  // 游戏结束，返回菜单
  if (this->state == GAME_OVER) {
    float totalStep = 180.0f;
    Renderer->RenderText("Game Over", glm::vec2(100, this->WindowHeight - 140), 1.0, glm::vec3(1, 0.6, 0.6));
    float current = UtilTool::scaleValue(loadStep / totalStep);
    this->camera->TransitionTo(getNowEyePos(), loadStep / totalStep);
    this->spriteSun->Postion.y = 25.0f - (20.0f * current);
    loadStep++;
    if (this->loadStep > totalStep) {
      this->keys[KEY_ENTER] = false;
      this->state = GAME_MENU;
      this->loadStep = 0;
    }
  }

  // 工具栏
  //{
  //  ImGui::Begin("Menu");
  //  if (ImGui::CollapsingHeader("Base")) {
  //    ImGui::ColorEdit3("Background color", (float*)& clearColor);
  //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  //  }
  //}
}

void GameApp::updateSnake() {
  int currentLen = this->spriteSnake->GetLenght();
  this->spriteSnake->Update(true, this->state == GAME_ACTIVE ? this->cheat : true);
  if (this->spriteSnake->Over()) {
    if (this->state == GAME_ACTIVE) {
      this->state = GAME_OVER;
    } else {
      this->spriteSnake->Reset();
    }
  }
  else if (this->spriteSnake->GetLenght() != currentLen) {
    if (this->spriteSnake->GetLenght() % 7 == 3 && updateFrameCount > 5) {
      this->updateFrameCount--;
    }
  }
}

void GameApp::updateGame() {
  this->spriteSnake->Update(false);
  // 菜单
  if (this->state == GAME_MENU) {
    // 展示智能寻路动画
    if (this->first) {
      this->updateSnake();
    }
  }

  // 游戏进行中
  if (this->state == GAME_ACTIVE) {
    this->updateSnake();
  }

  // 游戏已结束
  if (this->state == GAME_OVER) {
    this->keys[KEY_ENTER] = false;
  }
}

// 处理输入
void GameApp::processInput() {
  // 获取按键状态
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  this->keys[KEY_UP] = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
  this->keys[KEY_DOWN] = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
  this->keys[KEY_LEFT] = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  this->keys[KEY_RIGHT] = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

  this->keys[KEY_0] = glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS;
  this->keys[KEY_1] = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS;
  this->keys[KEY_2] = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS;
  this->keys[KEY_3] = glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS;
  this->keys[KEY_4] = glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS;
  this->keys[KEY_5] = glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS;
  this->keys[KEY_6] = glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS;
  this->keys[KEY_7] = glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS;
  this->keys[KEY_8] = glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS;
  this->keys[KEY_9] = glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS;

  this->keys[KEY_P] = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
  this->keys[KEY_ENTER] = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

  // 处理游戏逻辑
  // 作弊模式
  if (this->keys[KEY_0]) this->cheat = true;
  if (this->keys[KEY_UP] ||
    this->keys[KEY_DOWN] ||
    this->keys[KEY_LEFT] ||
    this->keys[KEY_RIGHT] ||
    this->keys[KEY_ENTER]) {
    this->cheat = false;
  }
  // 切换皮肤
  if (this->keys[KEY_3]) this->spriteSnake->SetHeadStyle(0); 
  if (this->keys[KEY_4]) this->spriteSnake->SetHeadStyle(1);
  if (this->keys[KEY_5]) this->spriteSnake->SetHeadStyle(2);
  if (this->keys[KEY_6]) this->spriteSnake->SetBodyStyle(0);
  if (this->keys[KEY_7]) this->spriteSnake->SetBodyStyle(1);
  if (this->keys[KEY_8]) this->spriteSnake->SetBodyStyle(2);

  // 菜单
  if (this->state == GAME_MENU) {
    if (this->keys[KEY_ENTER]) {
      this->first = false;
      this->keys[KEY_ENTER] = false;
      this->resetGame();
      this->state = GAME_LOAD;
    }
  }

  // 游戏暂停中
  if (this->state == GAME_PAUSE) {
    // 继续游戏
    for (int i = 0; i < 5; i++) {
      if (this->keys[i]) {
        this->keys[KEY_P] = false;
        this->state = GAME_ACTIVE;
        break;
      }
    }
  }

  // 游戏进行中
  if (this->state == GAME_ACTIVE) {
    // 暂停游戏
    if (this->keys[KEY_P]) {
      this->keys[KEY_P] = false;
      this->state = GAME_PAUSE;
    }
    // 方向控制
    if (this->keys[KEY_UP]) this->spriteSnake->SetNextDir(DIR_UP);
    if (this->keys[KEY_DOWN]) this->spriteSnake->SetNextDir(DIR_DOWN);
    if (this->keys[KEY_LEFT]) this->spriteSnake->SetNextDir(DIR_LEFT);
    if (this->keys[KEY_RIGHT]) this->spriteSnake->SetNextDir(DIR_RIGHT);
  }
}

// 重置状态
void GameApp::resetGame() {
  this->loadStep = 0;
  this->updateFrameCount = 14;
  this->spriteSnake->Reset();
}