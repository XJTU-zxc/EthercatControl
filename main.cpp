#include <iostream>
#include "tcp_drive.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
// #include "variabledef.h"
// #include "public.h"
#include "params.h"
#include <thread>
#include "common.h"
#include "ThreadSafeQueue.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include "paramscpp.h"
#include "timer.h"
void timerLoop()
{
    while (true)
    {
        DealWith_4096us();
        // 休眠 40 微秒
        std::this_thread::sleep_for(std::chrono::microseconds(400));
    }
}

int main(){
    /* 参数初始化 */
    GlobalParamsInit();
    InitPara();
    /* TCP初始化 */
    std::thread serverThread(startServer);
    serverThread.detach(); //TCP线程
    /* 主战程序初始化 */


    /*  分布式时钟同步 */
    

    /* 定时器任务模块 */
    std::thread timerThread(timerLoop);
    timerThread.detach(); // 分离线程，后台运行
    /* 背景程序 */
    while(1){
        /*  检查队列是否有内容 */
        if(!threadSafeQueue.empty()){
            RespMessage msg = threadSafeQueue.pop();
            /* 处理当前三指令 */
            handleThreeWords(msg);
            /* 将需要发送的消息添加进线程安全队列2中 */
            threadSafeQueue2.push(g_message_cpu2arm);
        }

        /* 别的模块的函数 */

    }

}