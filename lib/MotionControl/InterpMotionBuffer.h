#ifndef __INTERPMOTIONBUFFER_H__
#define __INTERPMOTIONBUFFER_H__

#include "InterpMotionBlock.h"
#include "InterpData.h"
#include "DDAOutput.h"
#include "motioncontrolparams.h"

enum
{
    MAX_INTERP_MOTION_BUFFER_SIZE = 5
};

/**
 * @brief 如果收到了指令 BGMS ，InterpMotionBuffer 会不断的从 InterpDataQueue 中读取插补数据，并执行
 *
 */
class InterpMotionBuffer
{
public:
    InterpMotionBuffer() : head(0), len(0), free(MAX_INTERP_MOTION_BUFFER_SIZE) {}

    /**
     * @brief 该函数在主程序中调用
     */
    bool Push(InterpData *idata)
    {
        if (free == 0)
        {
            return false;
        }

        int i = (head + len) % MAX_INTERP_MOTION_BUFFER_SIZE;

        interpMotionBlocks[i].Init(idata);
        if (interpMotionBlocks[i].Type() == INTERP_TYPE_LINE)
        {
            /* 直线插补会做一次速度规划 */
            interpMotionBlocks[i].Plan();
        }

        --free;
        ++len;

        return true;
    }

    /**
     * @brief
     */
    bool Execute()
    {
        if (this->len == 0)
        {
            return true;
        }

        bool isOk = interpMotionBlocks[head].Execute();

        if (interpMotionBlocks[head].Type() == INTERP_TYPE_LINE)
        {
            /* 如果直线插补当中，STM所有轴，则停止该插补并视为完成 */
            if (g_params_stm == 1 ||
                /* 两个运动轴都有限位说明两个轴都停止了 */
                (((InterplastLimit[g_params_plate_interp_axis[0] * 2] == 1) || (InterplastLimit[g_params_plate_interp_axis[0] * 2 + 1] == 1)) && ((InterplastLimit[g_params_plate_interp_axis[1] * 2] == 1) || (InterplastLimit[g_params_plate_interp_axis[1] * 2 + 1] == 1))))
            {
                isOk = true;
            }
            /* 如果是直线插补，需要输出 */
            dda.Out(&interpMotionBlocks[head]);
        }
        else
        {
            // 记录插补信息wyf
            // circleParam[0] = interpMotionBlocks[head].edx;
            // circleParam[1] = interpMotionBlocks[head].edy;
            circleParam[2] = interpMotionBlocks[head].i;
            circleParam[3] = interpMotionBlocks[head].j;
            circleParam[4] = interpMotionBlocks[head].circle_type;
        }

        if (isOk)
        {
            /* 新一次圆弧插补指令，需要计算正切刀位置wyf */
            init_tan_K_flag = true;
            /* 插补执行完成 */
            dda.Init();
            head = (head + 1) % MAX_INTERP_MOTION_BUFFER_SIZE;
            --len;
            ++free;
        }

        return true;
    }

    int Size()
    {
        return this->len;
    }

    bool IsFull()
    {
        return this->free == 0;
    }

    bool IsEmpty()
    {
        return this->len == 0;
    }

    bool Clear()
    {
        this->head = 0;
        this->len = 0;
        this->free = MAX_INTERP_MOTION_BUFFER_SIZE;
        this->dda.Init();

        return true;
    }

private:
    int16_t head;
    int16_t len;
    int16_t free;
    DDAOutput dda;
    InterpMotionBlock interpMotionBlocks[MAX_INTERP_MOTION_BUFFER_SIZE];
};

#endif