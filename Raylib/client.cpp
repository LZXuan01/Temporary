#define WIN32_LEAN_AND_MEAN // 减少 Windows 头文件的包含
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <vector>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")

// 取消 Windows API 的宏定义
#undef CloseWindow
#undef ShowCursor
#undef LoadImage
#undef DrawText
#undef DrawTextEx
#undef PlaySound

// 将 raylib.h 包含在命名空间中
namespace raylib
{
#include "C:\\Users\\limzh\\Raylib\\thirdparty\\raylib.h"
}

#define PORT 8080
#define SERVER_IP "192.168.1.4"
#define BUFFER_SIZE 1024

struct Line
{
    raylib::Vector2 start;
    raylib::Vector2 end;
};

std::vector<Line> lines; // 存储所有线条
std::mutex linesMutex;   // 保护 lines 的互斥锁

int main()
{
    // 初始化 Winsock 库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return -1;
    }

    // 创建 TCP 套接字
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("Socket creation failed\n");
        WSACleanup();
        return -1;
    }

    // 设置服务器地址信息
    sockaddr_in serv_addr = {AF_INET, htons(PORT)};
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
    {
        printf("Connection failed\n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    printf("Connected to server\n");

    // 将 Socket 设置为非阻塞模式
    u_long mode = 1; // 1 表示非阻塞模式
    ioctlsocket(sock, FIONBIO, &mode);

    // 初始化 Raylib 窗口
    raylib::InitWindow(800, 450, "Client");
    raylib::SetTargetFPS(60);

    raylib::Vector2 prevMousePos = {0, 0};
    bool isDrawing = false;

    while (!raylib::WindowShouldClose())
    {
        // 检查 Socket 是否有数据可读
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);

        struct timeval timeout = {0, 0}; // 非阻塞模式，立即返回
        int selectResult = select(0, &readSet, nullptr, nullptr, &timeout);

        if (selectResult > 0 && FD_ISSET(sock, &readSet))
        {
            // 有数据可读
            char buffer[BUFFER_SIZE];
            int valread = recv(sock, buffer, BUFFER_SIZE, 0);
            if (valread > 0)
            {
                buffer[valread] = '\0'; // 添加字符串结束符

                // 解析接收到的线条数据
                float x1, y1, x2, y2;
                if (sscanf(buffer, "LINE %f %f %f %f", &x1, &y1, &x2, &y2) == 4)
                {
                    std::lock_guard<std::mutex> lock(linesMutex); // 加锁
                    lines.push_back({{x1, y1}, {x2, y2}});        // 添加新线条
                }
            }
            else if (valread == 0)
            {
                // 服务器断开连接
                printf("Server disconnected\n");
                break;
            }
            else
            {
                // 接收数据出错
                printf("recv failed\n");
                break;
            }
        }

        // 鼠标绘制逻辑
        if (raylib::IsMouseButtonDown(raylib::MOUSE_LEFT_BUTTON))
        {
            raylib::Vector2 currentMousePos = raylib::GetMousePosition();
            if (isDrawing)
            {
                // 将线条数据发送到服务器
                char buffer[BUFFER_SIZE];
                snprintf(buffer, BUFFER_SIZE, "LINE %f %f %f %f",
                         prevMousePos.x, prevMousePos.y, currentMousePos.x, currentMousePos.y);
                send(sock, buffer, strlen(buffer), 0);
            }
            prevMousePos = currentMousePos;
            isDrawing = true;
        }
        else
        {
            isDrawing = false;
        }

        // Raylib 渲染
        raylib::BeginDrawing();
        raylib::ClearBackground(raylib::RAYWHITE);

        // 绘制所有线条
        std::lock_guard<std::mutex> lock(linesMutex); // 加锁
        for (const auto &line : lines)
        {
            raylib::DrawLineV(line.start, line.end, raylib::RED);
        }

        raylib::EndDrawing();
    }

    // 关闭连接
    closesocket(sock);
    WSACleanup();
    raylib::CloseWindow();
    return 0;
}