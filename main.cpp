#include "tcp_drive.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
// #include "public.h"
#include "params.h"
#include <thread>
#include "common.h"
#include "ThreadSafeQueue.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include "paramscpp.h"
#include "timer.h"
#include "igh_ethercat.h"

EthercatMaster *curIgHInstance;
bool motionFlag = false; // 是否通过IgH输出EtherCAT数据
/* Period */
const struct timespec cycletime = {0, PERIOD_NS};
struct timespec wakeupTime;
void cycle_task()
{
    while (true)
    {
        DealWith_4096us();
        std::this_thread::sleep_for(std::chrono::microseconds(4000));
        // if (motionFlag) {
        //     curIgHInstance->trans2OP();
        //     clock_gettime(CLOCK_TO_USE, &wakeupTime);
        //     curIgHInstance->set_all_position();
        //     wakeupTime = timespec_add(wakeupTime, cycletime);
        //     clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);
        // }
        // // 休眠 400 微秒
        // else
        // {
        //     std::this_thread::sleep_for(std::chrono::microseconds(400));
        // }
    }
}
int main()
{
    /* 参数初始化 */
    GlobalParamsInit();
    InitPara();

    /* TCP初始化 */
    std::thread serverThread(startServer);
    serverThread.detach();

    /* 主战程序初始化 */
    /*
     * igh Ethercat tool: Query the supported operating modes.
     * cmd: ethercat upload -t int32 -p 0 0x6502 00
     * result: 0x000003ad 941
     * support mode: csp tq pp
     * current mode: pp
     */
    // EthercatMaster igh_master;
    // if (igh_master.initialize() == -1)
    // {
    //     std::cerr << "Error initializing EtherCAT master." << std::endl;
    //     return 0;
    // }
    /*  分布式时钟同步 */
    /* 定时器任务模块 */
    std::thread timerThread(cycle_task);
    timerThread.detach(); // 分离线程，后台运行
    /* 背景程序 */
    while (1)
    {
        /*  检查队列是否有内容 */
        if (!threadSafeQueue.empty())
        {
            RespMessage msg = threadSafeQueue.pop();
            /* 处理当前三指令 */
            handleThreeWords(msg);
            /* 将需要发送的消息添加进线程安全队列2中 */
            std::cout<< "main函数打印消息内容：" <<std::endl;
            parseMessage(g_message_cpu2arm, CStringGetLen(g_message_cpu2arm));
            // threadSafeQueue2.push(g_message_cpu2arm);
            // 传递g_message_cpu2arm的内容副本
            threadSafeQueue2.push(std::string(g_message_cpu2arm, CStringGetLen(g_message_cpu2arm)));
        }
    }
}