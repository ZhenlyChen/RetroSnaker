
#include <stdio.h>
#include <Winsock2.h> 
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <glm/glm.hpp>

using namespace std;
using namespace chrono;
#pragma comment(lib, "ws2_32.lib")

enum SnakeDir { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
const int MoveX[4] = { 0 , 0, -1, 1 };
const int MoveY[4] = { -1, 1, 0, 0 };
const int MAX_X = 5;
const int MAX_Y = 6;

struct GameClient {
  sockaddr_in addr;
  bool ready = false;
  int connect = 0;

  SnakeDir nextDir;
  bool isLife = true;
  int snakeLength;
  int styleHead = 0;
  int styleBody = 0;
  int id = 0;
  glm::vec3 snakePos[300];
};

struct ServerData {
  glm::vec2 foodPos;
  unsigned char frame;
  unsigned char p;
  int styleFood = 0;
  bool map[MAX_X * 2 + 1][MAX_Y * 2 + 1];
};


class GameServer
{
public:
  GameServer() {
    
  }
  ~GameServer() {

  }

  int InitSocket(int port) {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
      return -1;
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
      cout << "Init socket error 1" << endl;
      return -1;
    }

    // 设置非阻塞
    char recvbuf = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof(int));
    u_long imode = 1;
    if (ioctlsocket(serverSocket, FIONBIO, &imode) != 0) {
      cout << "Init socket error 2" << endl;
      return -1;
    }
    // 设置地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY; // 任意地址
    serverAddr.sin_port = htons(port); // 端口

    if (bind(serverSocket, (SOCKADDR*)& serverAddr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
      cout << "Bind socket error" << endl;
      return -1;
    }
    listen(serverSocket, 5);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return 0;
  }

  void Listen() {
    while (true) {
      auto start = system_clock::now();
      auto duration = duration_cast<microseconds>(system_clock::now() - start);
      while (double(duration.count()) * microseconds::period::num < intervalSecond) {
        FD_ZERO(&rfd);              
        FD_SET(serverSocket, &rfd);
        int SelectRcv = select(serverSocket + 1, &rfd, 0, 0, &timeout);
        if (SelectRcv < 0)
          cout << "Fali: " << GetLastError() << endl;
        else if (SelectRcv > 0) {
          // 收到来自客户端的消息
          recvData();
        }
        duration = duration_cast<microseconds>(system_clock::now() - start);
        Sleep(1);
      }
      sendStatus();
      duration = duration_cast<microseconds>(system_clock::now() - updateTime);
      if (duration.count() > 0.5 * microseconds::period::den) {
        updateTime = system_clock::now();
        if (!running) continue;
        updateGame();
      }
    }
    closesocket(serverSocket);
    WSACleanup();
  }

private:
  SOCKET serverSocket;
  char recvBuf[1024];
  fd_set rfd;
  ServerData data;
  vector<GameClient> clients;
  sockaddr_in clientAddr;
  int addrLen = sizeof(SOCKADDR);
  double intervalSecond = 0.05 * microseconds::period::den;
  struct timeval timeout;
  system_clock::time_point updateTime;

  bool running = false;

  void recvData() {
    memset(recvBuf, 0, 1024);
    int recvLen = recvfrom(serverSocket, recvBuf, 1024, 0, (sockaddr*)& clientAddr, &addrLen);
    
    if (recvBuf[0] == 1) {
      // 客户端心跳维持
      handleNewClient();
    }
    else if (recvBuf[0] == 2) {
      // 客户端准备游戏
      handleReady();
    }
    else if (recvBuf[0] == 3) {
      // 处理客户端输入
      handleInput();
    }

  }

  void sendData(sockaddr_in& clientAddr, char* data, int len) {
    sendto(serverSocket, data, len + 1, 0, (sockaddr*)& clientAddr, addrLen);
  }


  void handleNewClient() {
    bool isExist = false;
    for (auto& c : clients) {
      if (c.addr.sin_addr.s_addr == clientAddr.sin_addr.s_addr && c.addr.sin_port == clientAddr.sin_port) {
        isExist = true;
        c.connect = 0;

        break;
      }
    }
    // 最多8位玩家
    if (!isExist && clients.size() < 8 && !running) {
      cout << "Client:" << clientAddr.sin_port << " join." << endl;
      clients.push_back({clientAddr, false, 0, DIR_UP, true, 0, 0, 0});
    }
    else if (!isExist && running) {
      char status[3] = { 9, 0, 0 };
      sendData(clientAddr, status, 3);
    }
  }

  void handleReady() {
    if (running) return;
    for (auto& c : clients) {
      if (c.addr.sin_addr.s_addr == clientAddr.sin_addr.s_addr && c.addr.sin_port == clientAddr.sin_port) {
        if (!c.ready) {
          cout << "Client:" << clientAddr.sin_port << " ready." << endl;
        }
        else {
          cout << "Client:" << clientAddr.sin_port << " cannel ready." << endl;
        }
        c.ready = recvBuf[1] == 1;
        break;
      }
    }

  }

  void handleInput() {
    char input = recvBuf[1];
    for (auto& c : clients) {
      if (c.addr.sin_addr.s_addr == clientAddr.sin_addr.s_addr && c.addr.sin_port == clientAddr.sin_port) {
        c.nextDir = (SnakeDir)input;
        break;
      }
    }
    cout << "Client:" << clientAddr.sin_port << " input: " << (int)input << endl;
  }

  // 同步游戏状态
  void sendStatus() {
    char readyClient = 0;
    // 移除掉线客户端
    clients.erase(
      std::remove_if(
        clients.begin(),
        clients.end(),
        [&](GameClient& c) -> bool {
          c.connect++;
          if (c.connect > 20) {
            cout << "Client:" << c.addr.sin_port << " quit." << endl;
            return true;
          }
          if (c.ready) readyClient++;
          return false;
        }
      ),
      clients.end());

    if (!running) {

      /*
      返回数据结构
      第一位
        1 - 游戏准备中
      第二位
        n - 服务器中玩家数量
      第三位
        n - 服务器中已准备玩家数量
      第四位
        1 - 当前玩家未准备
        2 - 当前玩家已经准备
      */
      char status[5] = {1, clients.size() , readyClient, 0, 0};
      for (auto& c : clients) {
        status[3] = c.ready + 1;
        sendData(c.addr, status, 5);
      }
      if (readyClient > 0 && readyClient == clients.size()) {
        startGame();
      }
    } else {

      /*
      返回数据结构
      第零位
        2 - 游戏运行中
      第一位
        x - 当前帧百分比
      第二位
        n - 服务器中玩家数量
      第三位
        n - 服务器中存活的玩家数量
      第四位
        1 - 当前玩家已死亡
        2 - 当前玩家未死亡
      第五位
        x - 当前玩家方向
      第六位
        n - 当前玩家的数据序号
      第七、八、九位
        x - 食物的x坐标
        y - 食物的y坐标
        t - 食物的种类
      [4]
        t - 蛇头的种类
        t - 蛇身体的种类
        n1 - 蛇身长度1 (0-256)
        n2 - 蛇身长度2 (0-256) 长度为 len =  n1*256 + n2
      [len * 3]
        x - 蛇身x
        y - 蛇身y
        d - 方向
      */
      char status[4096] = { 2, data.frame, clients.size() };
      int lifePlayer = 0;
      int index = 10;
      int clientID = 0;
      for (auto& c : clients) {
        c.id = clientID;
        clientID++;
        status[index++] = (char)c.styleHead;
        status[index++] = (char)c.styleBody;
        if (!c.isLife) { // 已经死亡
          status[index++] = (unsigned char)0;
          status[index++] = (unsigned char)0;
          continue;
        }
        lifePlayer++;
        status[index++] = (unsigned char)(c.snakeLength / 256);
        status[index++] = (unsigned char)(c.snakeLength % 256);
        for (int i = 0; i < c.snakeLength; i++) {
          status[index++] = (char)c.snakePos[i].x;
          status[index++] = (char)c.snakePos[i].y;
          status[index++] = (char)c.snakePos[i].z;
        }
      }

      status[3] = lifePlayer;
      status[7] = (char)data.foodPos.x;
      status[8] = (char)data.foodPos.y;
      status[9] = (char)data.styleFood;

      for (auto& c : clients) {
        status[4] = c.isLife + 1;
        status[5] = c.nextDir;
        status[6] = c.id;
        sendData(c.addr, status, index);
      }

      if (lifePlayer == 0) {
        endGame();
      }
    }
  }

  void startGame() {
    cout << "All Ready!" << endl;
    Sleep(1000);
    for (auto& c : clients) {
      if (!c.ready) return;
    }
    cout << "Start Game!" << endl;
    running = true;
    memset(data.map, 0, sizeof(data.map));
    for (auto& c : clients) {
      c.isLife = true;
      c.styleBody = rand() % 3;
      c.styleHead = rand() % 3;
      c.snakeLength = 2;
      // 寻找位置
      int count = 0;
      while (true) {
        if (count++ > 10000) {
          running = false;
          break;
        }
        int dir = rand() % 4;
        int x = rand() % 9 - 4;
        int y = rand() % 9 - 4;
        c.snakePos[1] = glm::vec3(
          x,
          y,
          dir);
        c.snakePos[0] = glm::vec3(
          x + MoveX[dir],
          y + MoveY[dir],
          dir);
        c.nextDir = (SnakeDir)dir;
        if (data.map[(int)c.snakePos[0].x + MAX_X][(int)c.snakePos[0].y + MAX_Y] == false &&
          data.map[(int)c.snakePos[1].x + MAX_X][(int)c.snakePos[1].y + MAX_Y] == false) {
          data.map[(int)c.snakePos[0].x + MAX_X][(int)c.snakePos[0].y + MAX_Y] = true;
          data.map[(int)c.snakePos[1].x + MAX_X][(int)c.snakePos[1].y + MAX_Y] = true;
          break;
        }
      }
    }
    makeFood();
    updateTime = system_clock::now();
  }

  void makeFood() {
    cout << "Make a food." << endl;
    int xLen = (MAX_X * 2 + 1);
    int yLen = (MAX_Y * 2 + 1);

    int count = 0;
    while (true) {
      if (count++ > 10000) {
        running = false;
        break;
      }
      int x = rand() % xLen - MAX_X;
      int y = rand() % xLen - MAX_Y;
      if (data.map[x+MAX_X][y+MAX_X] == false &&
        data.foodPos.x != x && data.foodPos.y != y) {
        data.foodPos.x = x;
        data.foodPos.y = y;
        data.styleFood = rand() % 3;
        break;
      }
    }
  }

  void endGame() {
    cout << "Game over!" << endl;
    running = false;
    for (auto& c : clients) {
      c.ready = false;
    }
  }

  void updateGame() {
    memset(data.map, 0, sizeof(data.map));
    data.frame++;
    // 更新方向
    for (auto& c : clients) {
      if (!c.isLife) continue;
      if (
        (c.nextDir == DIR_UP && c.snakePos[0].z != DIR_DOWN) ||
        (c.nextDir == DIR_DOWN && c.snakePos[0].z != DIR_UP) ||
        (c.nextDir == DIR_LEFT && c.snakePos[0].z != DIR_RIGHT) ||
        (c.nextDir == DIR_RIGHT && c.snakePos[0].z != DIR_LEFT)) {
        c.snakePos[0].z = c.nextDir;
      }
      // 更新蛇体
      for (int i = c.snakeLength - 1; i > 0; i--) {
        c.snakePos[i] = c.snakePos[i - 1];
        data.map[(int)c.snakePos[i].x + MAX_X][(int)c.snakePos[i].y + MAX_Y] = true;
      }
    }
    // 更新蛇头
    for (auto& c : clients) {
      if (!c.isLife) continue;
      c.snakePos[0].x = c.snakePos[0].x + MoveX[(int)c.snakePos[0].z];
      c.snakePos[0].y = c.snakePos[0].y + MoveY[(int)c.snakePos[0].z];
      // 碰撞检测
      if (data.map[(int)c.snakePos[0].x + MAX_X][(int)c.snakePos[0].y + MAX_Y] == true) {
        data.map[(int)c.snakePos[0].x + MAX_X][(int)c.snakePos[0].y + MAX_Y] = true;
        c.isLife = false;
        continue;
      }
      // 判断出界
      if (c.snakePos[0].x > MAX_X || c.snakePos[0].x < -MAX_X) {
        c.isLife = false;
        continue;
      }
      else if (c.snakePos[0].y > MAX_Y || c.snakePos[0].y < -MAX_Y) {
        c.isLife = false;
        continue;
      }
      data.map[(int)c.snakePos[0].x + MAX_X][(int)c.snakePos[0].y + MAX_Y] = true;
      // 判断吃到食物
      if (c.snakePos[0].x == data.foodPos.x && c.snakePos[0].y == data.foodPos.y) {
        c.snakePos[c.snakeLength] = c.snakePos[c.snakeLength - 1];
        c.snakeLength++;
        makeFood();
      }
    }
  }
};

void main() {
  GameServer server;

  if (server.InitSocket(4000) == 0) {
    cout << "Server is listening in 4000" << endl;
    server.Listen();
  }
}