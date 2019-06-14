#pragma once
#include <string>
#include <tchar.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <thread>
using namespace std;
#pragma comment( lib, "ws2_32.lib" ) 
#define BUF_SIZE 4096

class Net
{
public:
  Net() {
  }

  ~Net() {

  }

  int InitSocket(const char* ip, int port) {
    // 初始化DLL
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
      cout << "Init error" << endl;
      return -1;
    }
    // 创建套接字
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
      return -1;
    }
    // 服务器地址信息
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servAddr.sin_addr.s_addr);
    servAddr.sin_port = htons(port);
    return 0;
  }

  void CloseSocket() {
    closesocket(sock);
    WSACleanup();
  }

  void Send(char* data, int len) {
    int res = sendto(sock, data, len + 1, 0, (struct sockaddr*) & servAddr, sizeof(servAddr));
  }

  char* GetState(int& strLen) {
    sockaddr fromAddr;
    int addrLen = sizeof(fromAddr);
    char buffer[BUF_SIZE] = { 0 };
    strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);
    while (strLen == -1) {
      strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);
      Sleep(1);
    }
    if (strLen < BUF_SIZE) {
      buffer[strLen] = 0;
    }
    return buffer;
  }

private:
  SOCKET sock;
  sockaddr_in servAddr; // 服务器地址
};
