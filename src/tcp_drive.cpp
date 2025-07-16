#include "tcp_drive.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include "paramscpp.h"

RespClient respClient;
/* 用于初始化 RespClient 中的消息接收缓冲区 theBuf ： 2k */
char g_respBuffer[2 * 1024 + sizeof(CString) + 1];

/* 用于初始化 RespClient 中的 bulkStack ,最多递归 8 层*/
char g_cqBuffer[8*sizeof(int) + sizeof(CircularQueue)];

const int PORT = 43100;
const int BUFFER_SIZE = 1024;

/* 启TCP动服务端 */
void startServer() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Can't create a socket. Exiting" << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // serverAddr.sin_addr.s_addr = inet_addr("192.168.100.201");
        serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
        return;
    }

    while(true){
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == -1) {
            std::cerr << "Error in accept(). Exiting" << std::endl;
            return;
        }

        std::cout << "Client connected" << std::endl;
        onConnectionInit();
        handleClient(clientSocket);
    }
    close(serverSocket);
}


/* 处理消息 */
void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == -1) {
            std::cerr << "Error in recv(). Exiting" << std::endl;
            break;
        } else if (bytesReceived == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;

        ProcessMessage(buffer,(uint16_t)bytesReceived);

// 检查队列中是否有消息需要发送

        while (!threadSafeQueue2.empty()) {
           char* reMsg = threadSafeQueue2.pop();

            // 发送消息
            ssize_t bytesSent = send(clientSocket, reMsg, std::strlen(reMsg), 0);
            if (bytesSent == -1) {
                std::cerr << "Error in send(). Exiting" << std::endl;
                break;
            }
        }
    }

    close(clientSocket);
}

/**
 * 建立连接后resp解析器的初始化
*/
int onConnectionInit(){
    RespClientInit(&respClient);
    respClient.theBuf = CStringInit(g_respBuffer, 2 * 1024 + sizeof(CString) + 1);
    respClient.bulkStack = CQInit(g_cqBuffer, 8 * sizeof(int) + sizeof(CircularQueue));
    // respClient.arg = tcpServer;
    respClient.ExecuteCommand = ExecuteCommand;
    return 0;
}

/**
 * 解析收到的数据
*/
int ProcessMessage(const char* buf, uint16_t len){
    int rc;
     // 1 读取数据到缓冲区
    memcpy(respClient.theBuf + CStringGetLen(respClient.theBuf), buf, len);
    CStringIncrLen(respClient.theBuf, len);

    // 2 开始解析数据
    rc = RespParse(&respClient);
    if (rc == RESP_RC_ERROR) {
        /* 清空接收缓冲区 */
        CStringRange(respClient.theBuf, 0, 0);
        // 如果接收出错，应该断开连接
        return -1;
    }
    return 0;
}


/**
 * 处理每一条指令并将其放入线程安全队列中
 * 
*/
void ExecuteCommand(RespMessage* msg, void* arg){
    /* 加入线程安全队列 */
        if (msg != nullptr) { // 确保指针非空
        // 创建一个新的RespMessage对象副本并加入队列
        threadSafeQueue.push(*msg); // 这里自动调用RespMessage的拷贝构造函数
    }
}

