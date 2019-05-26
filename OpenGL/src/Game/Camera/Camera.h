#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "../util/util.h"

// 默认参数
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.04f;
const float ZOOM = 45.0f;

enum CameraKey { KEY_CHANGE };

class Camera {
public:
  Camera(GLFWwindow* window);
  void Update();

  // Camera 属性
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Look;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // Euler 角度
  float Yaw;
  float Pitch;
  // 移动灵敏度
  float MovementSpeed;
  // 鼠标灵敏度
  float MouseSensitivity;
  // 滚轮缩放
  float Zoom;

  // 获取视图矩阵
  glm::mat4 GetViewMatrix();
  // 处理鼠标移动
  static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
  // 处理滚轮滚动
  static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  // 设置固定视角
  void SetLookPostion(glm::vec3 pos, glm::vec3 look = glm::vec3(0, 0, 0));
  // 设置自由视角
  void SetViewPostion(glm::vec3 pos, glm::vec3 front, glm::vec3 up = glm::vec3(0, 1, 0));
  // 平滑过渡摄像机位置
  void TransitionTo(glm::vec3 target, float p);

private:
  GLFWwindow* window;
  glm::vec3 oldPostion;
  bool keys[100];

  void processInput();
  void upDateDeltaTime();

  bool freedomView = false;

  float lastX;
  float lastY;
  bool firstMouse = true;
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
};
