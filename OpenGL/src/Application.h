#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lib/imgui/imgui.h"
#include "lib/imgui/imgui_impl_glfw.h"
#include "lib/imgui/imgui_impl_opengl3.h"

class Application {
public:
  // 初始化窗口并准备资源
  virtual void Init(GLFWwindow* window) = 0;
  // 更新
  virtual void Update() = 0;
  // 清除
  virtual void Clear() = 0;

  virtual void UpdateSize(int w, int h) = 0;

  void SetWindowStyle(std::string title, int h, int w) {
    this->WindowTitle = title;
    this->WindowHeight = h;
    this->WindowWidth = w;
  }

  std::string WindowTitle = "OpenGL";
  
  int WindowHeight = 800;

  int WindowWidth = 800;
};