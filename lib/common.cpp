#include "variabledef.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "common.h"
#include "params.h"
#include "compile.h"
#include "SpeedFilterQueue.h"
#include "ProgrammerControl/ProgrammerBulk.h"
#include "paramscpp.h"
#include "MotionControl/motioncontrolparams.h"
#include "ProgrammerControl/ProgrammerSystemParams.h"
#include "ThreadSafeQueue.h"
#include <cstring>

ProgrammerBulk g_programmerBulk;
SpeedFilterQueue g_speedFilterQueueA;
SpeedFilterQueue g_speedFilterQueueB;
SpeedFilterQueue g_speedFilterQueueC;
SpeedFilterQueue g_speedFilterQueueD;
SpeedFilterQueue g_speedFilterQueueE;
SpeedFilterQueue g_speedFilterQueueF;
SpeedFilterQueue g_speedFilterQueueG;
SpeedFilterQueue g_speedFilterQueueH;

struct DataStorage dataStorage = {0};
/* 线程安全队列 */
ThreadSafeQueue<RespMessage> threadSafeQueue(50); // 接受上位机发送的消息
ThreadSafeQueue<std::string> threadSafeQueue2(50); // 返回消息给上位机
// ThreadSafeQueue<CommandResult> threadSafeQueue2(50);       // 返回消息给上位机
/**
 * 初始化TCP，EtehrCAT，运动参数
 */
void Init()
{
    // TCP初始化

    // EtehrCAT初始化

    // 运动参数初始化
}

/* 检查 AIT 等待 */
int IsCommandLineWaitIO()
{
    if (g_params_commandline_ait_seq >= 0)
    {
        /* 需要等待 io */
        return 1;
    }

    return 0;
}

/* 检查 AMP 等待 */
int IsCommandLineWaitAMP()
{
    if ((g_params_commandline_amp & g_axis_is_running) > 0)
    {
        /* 需要等待轴运动完毕 */
        return 1;
    }

    return 0;
}

/* 检查命令行等待 */
int IsCommandLineWait()
{
    int rc = 0;
    /* 检查 IO 应答 */
    rc = IsCommandLineWaitIO();
    rc |= IsCommandLineWaitAMP();

    return rc;
}

/**
 * @brief 更改 IO 等待状态
 * */
int CheckCommandLineWaitIO()
{
    if (g_params_commandline_ait_seq >= 0)
    {
        if (g_params_commandline_ait_seq < 16)
        {
            int st = ((ThreeWordsReg.iIOLow >> g_params_commandline_ait_seq) & 0x01);
            if (st == g_params_commandline_ait_val)
            {
                g_params_commandline_ait_seq = -1;
            }
        }
        else
        {
            int st = ((ThreeWordsReg.iIOHigh >> (g_params_commandline_ait_seq - 16)) & 0x01);
            if (st == g_params_commandline_ait_val)
            {
                g_params_commandline_ait_seq = -1;
            }
        }
    }

    return 0;
}

/**
 * @brief 更改 AMP 等待状态
 * */
int CheckCommandLineWaitAMP()
{
    g_params_commandline_amp &= g_axis_is_running;

    return 0;
}

int CheckCommandLineWait()
{
    /* 改变 IO 状态 */
    CheckCommandLineWaitIO();
    /* 改变 AMP 状态 */

    CheckCommandLineWaitAMP();
    return 0;
}

int handleThreeWords(RespMessage msg)
{
    //    if (IsCommandLineWait() > 0) {
    //        /* 在等待期间，即使有数据，也不会产生应答 */
    //        /* 需要等待某些标志位，返回 */
    //        return 0;
    //    }
    // 清空 g_message_cpu2arm
    CStringClear(g_message_cpu2arm);
    if (msg.code == 0x4B)
    {
        /* 这是一条指令 */
        int rc = -1;
        /* 立马处理当前指令 */
        rc = CompilerParse(g_compilerPtr, (const char *)msg.data, msg.datalen, true);
        if (rc == -1)
        {
            /* 当前指令无法解析，直接报错 */
            // CStringAddRespFail(g_message_cpu2arm);
        }
    }
    else if (msg.code == 0x4C)
    {
        /* 暂停程序 */
        g_params_is_execute_prog = 0;
        /* 计数器清 0 */
        g_params_prog_counter = 0;
        /* 文件下传模式，清空程序缓冲区 */
        g_programmerBulk.Clear();
        /* 清空插补队列 */
        g_interpDataQueue.Clear();
        g_interpMotionBuffer.Clear();

        interpNum = 0;
        /* 返回消息 */
        // CStringAddRespFail(g_message_cpu2arm);
    }
    else if (msg.code == 0x4D)
    {
        /* 将数据保存到程序缓冲区中 */
        g_programmerBulk.AddLine((const char *)msg.data, msg.datalen);
        /* 返回消息 */
        // CStringAddRespOk(g_message_cpu2arm);
    }
    else if (msg.code == 0x4E)
    {
        // 目前该控制器还不能实现flash操作，后续全部暂时屏蔽
        /* 返回消息 */
        // CStringAddRespFail(g_message_cpu2arm);
    }
    else if (msg.code == 0x4F)
    {
        /* 返回消息 */
        // CStringAddRespFail(g_message_cpu2arm);
    }
    else if (msg.code == 0x50)
    {
        /* 返回消息 */
        // CStringAddRespFail(g_message_cpu2arm);
    }
    else if (msg.code == 0x51)
    {
        /* 返回消息 */
        // CStringAddRespFail(g_message_cpu2arm);
    }
    return 0;
}

/* 如果收到了 BGM 指令，处理运动指令 */
int ExecuteBGMCommand()
{
    if (g_params_bgm == 0)
    {
        return 0;
    }

    if (g_params_bgm >= 0x100)
    {
        /* 暂时只支持 A-H 的检测 */
        //        g_params_bgm = 0;
        return 0;
    }

    /* 设置为自动运行模式 */
    RunstatusRegs.RunStatusFlag.bit.Type = 5;
    /* 点位运动 */
    EncodeRegs.AsReg.Type = 0;
    /* 设置为正在运行 */
    RunstatusRegs.RunStatusFlag.bit.CBAFlag = 1;
    RunstatusRegs.RunStatusFlag.bit.AutoFlag = 1;
    for (int i = 0; i < SLAVE_NUM; ++i)
    {
        int16_t bits = (g_params_bgm & (1 << i));
        if (bits != 0)
        {
            if (g_axis_mvx_params[i].type == 1)
            {
                // 绝对值编程
                *g_params_axisPositionPtr[i] = g_axis_mvx_params[i].val;
            }
            else if (g_axis_mvx_params[i].type == 2)
            {
                // 相对值编程
                *g_params_axisPositionPtr[i] = *g_params_axisRealPositionPtr[i] + g_axis_mvx_params[i].val;
            }
            printf("[INFO] 轴[%d]的目标位置为: %d\n", i, *g_params_axisPositionPtr[i]);
            printf("[INFO] 轴[%d]当前设置速度为：%f\n", i, ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[i]);
            /* 当前轴处于运动状态 */
            g_axis_is_running |= (1 << i);
            g_axis_mvx_params[i].type = 0;
        }
    }
    /* 更新 g_params_bgm */
    g_params_bgm = 0;

    return 0;
}

/* 处理急停情况 */
int ExcuteEstCommand()
{
    /* 停所有轴*/
    if (g_params_interp_is_running > 0 || g_axis_is_running > 0)
    {
        // GPIO_writePin(103, 0);
        // GPIO_writePin(104, 0);
        // GPIO_writePin(95, 0);
        // GPIO_writePin(111, 0);
        // GPIO_writePin(30, 0);
        // GPIO_writePin(118, 0);
        // GPIO_writePin(117, 0);
        // GPIO_writePin(38, 0);
        g_axis_is_running = 0;
        g_params_interp_is_running = 0;
        g_params_mof = 0;
    }
    /* 清空插补轴 */
    g_params_interp_axis_enable = 0;
    /* 结束插补模式 */
    g_params_motion_mode = 0;
    /* 结束正切模式wyf */
    g_params_vtn_enable = 0;
    /* 清空插补队列和缓冲队列中的数据 */
    g_interpDataQueue.Clear();
    g_interpMotionBuffer.Clear();
    /* 清空平面轴信息 */
    g_params_plate_interp_axis[0] = -1;
    g_params_plate_interp_axis[1] = -1;
    g_params_plate_interp_axis[2] = -1;
    /* 清空正切联动轴信息 */
    g_params_vtn_axis[0] = -1;
    g_params_vtn_axis[1] = -1;
    g_params_vtn_axis[2] = -1;

    /* 暂停程序 */
    g_params_is_execute_prog = 0;
    /* 计数器清 0 */
    g_params_prog_counter = 0;
    g_params_wtm_count = 0; // 等待时间置位

    return 1;
}

int PushData()
{
    if (g_params_motion_mode == 1)
    {

        /* 使能了插补运动 */
        if (!g_interpMotionBuffer.IsFull())
        {
            InterpData *idata = g_interpDataQueue.Pop();
            if (idata != NULL)
            {
                g_interpMotionBuffer.Push(idata);
            }
        }
    }

    return 0;
}
