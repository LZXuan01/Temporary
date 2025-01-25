#define WIN32_LEAN_AND_MEAN // 减少 Windows 头文件的包含
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_set>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

std::unordered_set<SOCKET> clients; // 存储所有客户端套接字
std::mutex clientsMutex;            // 保护 clients 的互斥锁

// 广播数据给所有客户端
void broadcast(const char *data, SOCKET sender)
{
    std::lock_guard<std::mutex> lock(clientsMutex); // 加锁
    for (auto client : clients)
    {
        if (client != sender)
        { // 不发送给发送者
            send(client, data, strlen(data), 0);
        }
    }
}

// 处理客户端连接
void handleClient(SOCKET clientSocket)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        int valread = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0)
        { // 客户端断开连接
            break;
        }
        buffer[valread] = '\0'; // 添加字符串结束符
        printf("Received: %s\n", buffer);

        // 广播接收到的数据给其他客户端
        broadcast(buffer, clientSocket);
    }

    // 客户端断开连接，移除套接字
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(clientSocket);
    closesocket(clientSocket);
    printf("Client disconnected\n");
}

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
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET)
    {
        printf("Socket creation failed\n");
        WSACleanup();
        return -1;
    }

    // 设置服务器地址信息
    sockaddr_in address = {AF_INET, htons(PORT), INADDR_ANY};
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        printf("Bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    // 监听连接
    if (listen(server_fd, 3) == SOCKET_ERROR)
    {
        printf("Listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    printf("Server listening on port %d\n", PORT);

    while (true)
    {
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        SOCKET new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_socket == INVALID_SOCKET)
        {
            printf("Accept failed\n");
            continue;
        }

        // 将新客户端加入集合
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.insert(new_socket);
        printf("New client connected\n");

        // 创建一个新线程处理客户端
        std::thread(handleClient, new_socket).detach();
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}