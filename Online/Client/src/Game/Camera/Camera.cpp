
#include"Camera.h"

static Camera* CameraInst;


Camera::Camera(GLFWwindow* window) {
  CameraInst = this;

  this->Position = glm::vec3(0.0f, 3.0f, 10.0f);
  this->Front = glm::vec3(0.0f, 0.0f, -1.0f);
  this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
  this->Yaw = YAW;
  this->Pitch = PITCH;
  this->MovementSpeed = SPEED;
  this->MouseSensitivity = SENSITIVITY;
  this->Zoom = ZOOM;
  this->freedomView = false;

  this->window = window;
  glfwSetCursorPosCallback(window, this->MouseCallback);
  glfwSetScrollCallback(window, this->ScrollCallback);
}

glm::mat4 Camera::GetViewMatrix() {
  if (this->freedomView) {
    return glm::lookAt(Position, Position + Front, Up);
  } else {
    return glm::lookAt(Position, Look, Up);
  }
}

void Camera::Update() {
  this->processInput();
  this->upDateDeltaTime();
}

void Camera::SetLookPostion(glm::vec3 pos, glm::vec3 look) {
  this->Position = pos;
  this->Look = look;
}

void Camera::SetViewPostion(glm::vec3 pos, glm::vec3 front, glm::vec3 up) {
  this->Position = pos;
  this->Front = front;
  this->Up = up;
}

void Camera::TransitionTo(glm::vec3 target, float p) {
  if (p < 0.1) {
    this->oldPostion = this->Position;
  }
  else {
    float current = UtilTool::scaleValue(p);
    this->Position = this->oldPostion - (this->oldPostion - target) * current;
  }
}

void Camera::upDateDeltaTime() {
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
}

void Camera::processInput() {

  // 自由视角下键盘控制摄像机方向
  if (this->freedomView) {
    float velocity = MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      Position += Front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      Position -= Front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      Position -= Right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      Position += Right * velocity;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
      Position += Up * velocity;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
      Position -= Up * velocity;
  }
  // 切换模式
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
    if (keys[KEY_CHANGE] == false) {
      if (this->freedomView) {
        // 释放鼠标
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        this->firstMouse = true;
      } else {
        // 捕获鼠标
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
      this->freedomView = !this->freedomView;
      keys[KEY_CHANGE] = true;
    }
  }
  else {
    keys[KEY_CHANGE] = false;
  }
}

void Camera::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (!CameraInst->freedomView) return;
  if (CameraInst->firstMouse) {
    CameraInst->lastX = xpos;
    CameraInst->lastY = ypos;
    CameraInst->firstMouse = false;
  }

  float xoffset = xpos - CameraInst->lastX;
  float yoffset = CameraInst->lastY - ypos;

  CameraInst->lastX = xpos;
  CameraInst->lastY = ypos;

  xoffset *= CameraInst->MouseSensitivity;
  yoffset *= CameraInst->MouseSensitivity;

  CameraInst->Yaw += xoffset;
  CameraInst->Pitch += yoffset;

  if (CameraInst->Pitch > 89.0f)
    CameraInst->Pitch = 89.0f;
  if (CameraInst->Pitch < -89.0f)
    CameraInst->Pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(CameraInst->Yaw)) * cos(glm::radians(CameraInst->Pitch));
  front.y = sin(glm::radians(CameraInst->Pitch));
  front.z = sin(glm::radians(CameraInst->Yaw)) * cos(glm::radians(CameraInst->Pitch));
  CameraInst->Front = glm::normalize(front);
  CameraInst->Right = glm::normalize(glm::cross(CameraInst->Front, CameraInst->WorldUp));
  CameraInst->Up = glm::normalize(glm::cross(CameraInst->Right, CameraInst->Front));
}

void Camera::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  if (!CameraInst->freedomView) return;
  if (CameraInst->Zoom >= 1.0f && CameraInst->Zoom <= 45.0f)
    CameraInst->Zoom -= yoffset;
  if (CameraInst->Zoom <= 1.0f)
    CameraInst->Zoom = 1.0f;
  if (CameraInst->Zoom >= 45.0f)
    CameraInst->Zoom = 45.0f;
}
