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
  this->mySnake = new SpriteSnake(this->Renderer, MAX_X, MAX_Y);
  this->spriteSun = new SpriteSun(this->Renderer);
  this->spriteGarden = new SpriteGarden(this->Renderer);
  this->spriteTable = new SpriteTable(this->Renderer);
  this->spriteFood = new SpriteFood(this->Renderer);
  // 初始化背景颜色
  this->clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  // 初始化摄像机
  this->camera = new Camera(window);
  this->camera->SetLookPostion(glm::vec3(20, 20, 30));
  // 初始化状态
  this->state = GAME_MENU;

  if (net.InitSocket("127.0.0.1", 4000) == 0) {
    // 接受数据线程
    recThread = std::thread([&]() {
      while (true) {
        int statusLen;
        char* status = net.GetState(statusLen);
        mtx.lock();
        memcpy_s(&serverStatus, 4096, status, statusLen);
        //for (int i = 0; i < 20; i++) {
        //  cout << (int)serverStatus[i] << " ";
        //}
        //cout << endl;
        mtx.unlock();
      }
      });
    // 心跳维持线程
    heartThread = std::thread([&]() {
      while (true) {
        char heard[2] = { 1, 0 };
        net.Send(heard, 2);
        Sleep(300);
      }
      });
  } else {
    cout << "无法连接服务器" << endl;
  }
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
  int snakeIndex = 0;
  for (auto& snake : this->snakes) {
    // printf("%d - %d\n", snakeIndex, myIndex);
    snake->Render(this->frame / (float)this->updateFrameCount, snakeIndex == myIndex);
    snakeIndex++;
  }

  this->spriteFood->Render(this->frame / (float)this->updateFrameCount);
  this->spriteSun->Render();
  this->spriteGarden->Render();
  this->spriteTable->Render();

  // 菜单场景
  if (this->state == GAME_MENU) {
    Renderer->RenderText("3D Happy Snake Online", glm::vec2(100, this->WindowHeight - 100), 1.2);
    if (isNet && !isBusy) {
      Renderer->RenderText("Ready player: " + to_string(this->readyCount) + " / " + to_string(this->playerCount),
        glm::vec2(100, this->WindowHeight - 170), 0.5);
      Renderer->RenderText(this->isReady ? "Ready! Waiting other player..." : "Press ENTER to ready game!",
        glm::vec2(100, this->WindowHeight - 200), 0.5,
        this->isReady ? glm::vec3(0.8, 1.0, 1.0) : glm::vec3(1.0, 0.8, 0.8));
    }
    if (!isBusy) {
      Renderer->RenderText(this->isNet ? "Status: Online" : "Status: Connecting to server...", glm::vec2(10, 10), 0.5);
    }
    else {
      Renderer->RenderText("Status: A game is running.", glm::vec2(10, 10), 0.5);
    }

    if (this->mySnake->GetLenght() > 1 && this->first == false) {
      Renderer->RenderText("Your score: " + std::to_string(this->mySnake->GetLenght() - 2), glm::vec2(100, this->WindowHeight - 250), 0.8);
    }
  }

  // 加载中动画
  if (this->state == GAME_LOAD) {
    float totalStep = 60.0f;
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
      // this->state = GAME_ACTIVE;
      this->loadStep = 0;
    }
  }

  // 游戏开始
  if (this->state == GAME_ACTIVE) {
    this->camera->SetLookPostion(glm::vec3(0, 10, 17));
    if (isOver) {
      Renderer->RenderText(
        "You had over!",
        glm::vec2(40, this->WindowHeight - 60), 1.0, glm::vec3(1,0.7,0.7));

    }
    else {
      Renderer->RenderText(
        "Your score: " + std::to_string(this->mySnake->GetLenght() - 2),
        glm::vec2(40, this->WindowHeight - 60));

    }
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
}
void GameApp::updateGame() {
  // 游戏进行中
  if (this->lastFrame != this->statusframe) {
    for (auto& snake : snakes) {
      snake->Update();
    }
    this->spriteFood->Update();
    this->lastFrame = this->statusframe;
  }
  //if (this->state == GAME_ACTIVE) {
  //  if (this->lastFrame != this->statusframe) {
  //    this->spriteSnake->Update();
  //    this->spriteFood->Update();
  //    this->lastFrame = this->statusframe;
  //  } 
  //}


  // 从服务端更新游戏数据
  mtx.lock();
  this->isBusy = this->serverStatus[0] == 9;
  this->isNet = this->serverStatus[0] != 0;
  if (this->serverStatus[0] == 1) {
    // 游戏准备中
    this->state = GAME_MENU;
    //if (this->state == GAME_ACTIVE) {
    //  this->state = GAME_OVER;
    //}
    this->spriteFood->First = true;
    this->isReady = this->serverStatus[3] == 2;
    this->playerCount = this->serverStatus[1];
    this->readyCount = this->serverStatus[2];
  }
  else if (this->serverStatus[0] == 2) {
    // 游戏进行中
    // this->p = this->serverStatus[6] / 256.0;
    this->statusframe = this->serverStatus[1];
    this->playerCount = this->serverStatus[2];
    this->lifeCount = this->serverStatus[3];
    this->isOver = this->serverStatus[4] == 1;
    this->snakeDir = (SnakeDir) this->serverStatus[5];
    this->myIndex = this->serverStatus[6];
    // printf("this state %d\n", this->state);
    if (this->state != GAME_ACTIVE) {
      this->state = GAME_ACTIVE;
      this->snakes.clear();
      for (int i = 0; i < playerCount; i++) {
        SpriteSnake* s = new SpriteSnake(this->Renderer, MAX_X, MAX_Y);
        s->First = true;
        this->snakes.push_back(s);
      }
    }


    this->spriteFood->SetData(glm::vec2(this->serverStatus[7], this->serverStatus[8]), this->serverStatus[9]);
    if (this->spriteFood->First == true) {
      this->spriteFood->First = false;
      this->spriteFood->Update();
    }

    int index = 10;
    if (!snakes.empty()) {
      for (int i = 0; i < this->playerCount; i++) {
        vector<glm::vec3> pos;
        int head = serverStatus[index++];
        int body = serverStatus[index++];
        int len = (unsigned char)serverStatus[index++] * 256;
        len += (unsigned char)serverStatus[index++];
        for (int j = 0; j < len; j++) {
          int x = serverStatus[index++];
          int y = serverStatus[index++];
          int d = serverStatus[index++];
          pos.push_back(glm::vec3(x, y, d));
        }
        int oldLen = this->snakes[i]->GetLenght();
        this->snakes[i]->SetData(pos, len, head, body);
        if (len != oldLen) {
          this->snakes[i]->Update(true);
        }
        if (this->snakes[i]->First == true) {
          this->snakes[i]->First = false;
          this->snakes[i]->Update();
          this->snakes[i]->Dir = this->snakeDir;
        }
      }
      mySnake = snakes[myIndex];
    }
  }
  mtx.unlock();


  if (this->state == GAME_ACTIVE) {
    if (this->mySnake->Dir != this->snakeDir) {
      char status[3] = { 3, (char)this->mySnake->Dir, 0 };
      net.Send(status, 3);
    }
  }


  if (this->state == GAME_MENU) {
    if (playerCount == readyCount) {
      this->state = GAME_LOAD;
    }
  }
}

// 处理输入
void GameApp::processInput() {

  // 菜单
  if (this->state == GAME_MENU) {
    if (this->keys[KEY_ENTER] == false &&
      (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {
      if (isReady) {
        char status[3] = { 2, 0, 0 }; // 取消准备游戏
        net.Send(status, 3);
      }
      else {
        char status[3] = { 2, 1, 0 }; // 准备游戏
        net.Send(status, 3);
      }
    }
  }

  // 游戏进行中
  if (this->state == GAME_ACTIVE) {
    // 方向控制
    if (this->keys[KEY_UP]) {
      this->mySnake->Dir = DIR_UP;
    }
    if (this->keys[KEY_DOWN]) {
      this->mySnake->Dir = DIR_DOWN;
    }
    if (this->keys[KEY_LEFT]) {
      this->mySnake->Dir = DIR_LEFT;
    }
    if (this->keys[KEY_RIGHT]) {
      this->mySnake->Dir = DIR_RIGHT;
    }
  }

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
}
