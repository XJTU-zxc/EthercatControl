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
char g_cqBuffer[8 * sizeof(int) + sizeof(CircularQueue)];

const int PORT = 43100;
const int BUFFER_SIZE = 1024;

/* 启动TCP服务端 */
void startServer()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Can't create a socket. Exiting" << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // serverAddr.sin_addr.s_addr = inet_addr("192.168.100.201");
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        return;
    }

    while (true)
    {
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);

        if (clientSocket == -1)
        {
            std::cerr << "Error in accept(). Exiting" << std::endl;
            return;
        }

        std::cout << "Client connected" << std::endl;
        onConnectionInit();
        handleClient(clientSocket);
    }
    close(serverSocket);
}

// @author zxc
// @description
//    原消息处理使用了队列，但目前是单客户端，指令解析和处理也很快，不需要并发处理，暂不需要队列
//    目前的修改保留了队列操作，以备后续扩展，实际处理仍然是一对一阻塞单线程
// @revision 1.0 by zxc on 2025-07-21
void handleClient(int clientSocket)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == -1)
        {
            std::cerr << "Error in recv(). Exiting" << std::endl;
            break;
        }
        else if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;

        // 处理接收到的消息
        ProcessMessage(buffer, (uint16_t)bytesReceived);

        // 等待队列中有数据
        while (true)
        {
            if (!threadSafeQueue2.empty())
            {
                // 从队列中弹出数据
                std::string reMsg = threadSafeQueue2.pop();
                printf("发送消息：%s\n", reMsg.c_str());

                // 发送消息给客户端
                ssize_t bytesSent = send(clientSocket, reMsg.c_str(), reMsg.size(), 0);
                if (bytesSent == -1)
                {
                    std::cerr << "Error in send(). Exiting" << std::endl;
                    break;
                }
                break; // 发送成功后跳出等待循环
            }
        }
        // while (true) {
        //     if (!threadSafeQueue2.empty()) {
        //         CommandResult cmdRes = threadSafeQueue2.pop();
        //         if (cmdRes.commandId == currentCommandId) {
        //             printf("发送消息：%s\n", cmdRes.result.c_str());
        //             // 发送消息
        //             ssize_t bytesSent = send(clientSocket, cmdRes.result.c_str(), cmdRes.result.size(), 0);
        //             if (bytesSent == -1)
        //             {
        //                 std::cerr << "Error in send(). Exiting" << std::endl;
        //                 break;
        //             }
        //             break; // 找到匹配结果，跳出循环
        //         } else {
        //             // 若不是当前命令的结果，重新放回队列
        //             threadSafeQueue2.push(cmdRes);
        //         }
        //     }
        // }
    }
    close(clientSocket);
}

/**
 * 建立连接后resp解析器的初始化
 */
int onConnectionInit()
{
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
int ProcessMessage(const char *buf, uint16_t len)
{
    int rc;
    // 1 读取数据到缓冲区
    memcpy(respClient.theBuf + CStringGetLen(respClient.theBuf), buf, len);
    CStringIncrLen(respClient.theBuf, len);

    // 2 开始解析数据
    rc = RespParse(&respClient);
    if (rc == RESP_RC_ERROR)
    {
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
void ExecuteCommand(RespMessage *msg, void *arg)
{
    /* 加入线程安全队列 */
    if (msg != nullptr)
    { // 确保指针非空
        // 创建一个新的RespMessage对象副本并加入队列
        threadSafeQueue.push(*msg); // 这里自动调用RespMessage的拷贝构造函数
    }
}
