#pragma once
#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <time.h>
#include "../Application.h"
#include "../lib/imgui/imgui.h"

#include "Camera/Camera.h"
#include "ResourceManager/ResourceManager.h"
#include "SpriteRenderer/SpriteRenderer.h"
#include "Util/Util.h"
#include "AI/SmartSnake.h"

#include "Sprite/SpriteSnake.h"
#include "Sprite/SpriteSun.h"
#include "Sprite/SpriteGarden.h"
#include "Sprite/SpriteTable.h"
#include "Sprite/SpriteFood.h"
#include "Net/Net.h"
#include <shared_mutex>
#include <thread>
#include <mutex>

enum GameState {GAME_ACTIVE, GAME_MENU, GAME_LOAD, GAME_OVER, GAME_PAUSE};
enum KEY {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_P, 
  KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9};

class GameApp : public Application {
public:
  GameApp();
  void Init(GLFWwindow* window);
  void Update();
  void Clear();
  void UpdateSize(int w, int h);

  std::string WindowTitle = "3D Happy Snake";
  int WindowHeight = 800;
  int WindowWidth = 1024;
private:
  // 处理输入
  void processInput();
  // 渲染
  void render();
  // 更新游戏逻辑
  void updateGame();

  // 游戏参数
  GLFWwindow* window;
  ImVec4 clearColor;
  // 更新频率
  int frame = 0;
  int loadStep = 0;
  int updateFrameCount = 8;
  // 首次加载
  bool first = true;
  // 开启作弊
  bool cheat = false;
  // 输入数据
  bool keys[128];

  // 游戏状态
  GameState state;
  // 精灵渲染
  SpriteRenderer* Renderer;
  // 摄像机
  Camera* camera;
  // 游戏对象
  vector<SpriteSnake*> snakes;
  SpriteSnake* mySnake;

  float p = 0.0;



  SpriteFood* spriteFood;
  SpriteSun* spriteSun;
  SpriteTable* spriteTable;
  SpriteGarden* spriteGarden;

  bool isReady = false;
  bool isNet = false;
  bool isBusy = false;
  bool isOver = false;
  int playerCount = 1;
  int lifeCount = 1;
  int readyCount = 0;
  unsigned char statusframe;
  unsigned char lastFrame;
  SnakeDir snakeDir;
  int myIndex;



  Net net;

  std::thread recThread;
  std::thread heartThread;
  std::mutex mtx;
  char serverStatus[4096] = {0};
};

#endif // !GAME_H
