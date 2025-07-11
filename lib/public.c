#include "public.h"
#include <math.h>


RobotCore robotCore;


uint16_t HWReadEncode(uint16_t* address)
{
    uint16_t res = 0;
    res = *address;
    return res;
}

void RobotCoreInitEncode(RobotCore* core)
{
    uint16_t x = HWReadEncode((uint16_t *)0x00002007);  // 读取 x 轴编码器读数
    uint16_t y = HWReadEncode((uint16_t *)0x00002009);  // 读取 y 轴编码器读数
    uint16_t z = HWReadEncode((uint16_t *)0x0000200A);  // 读取 z 轴编码器读数
    uint16_t a = HWReadEncode((uint16_t *)0x0000200B);  // 读取 a 轴编码器读数
    uint16_t b = HWReadEncode((uint16_t *)0x0000200C);  // 读取 b 轴编码器读数
    uint16_t c = HWReadEncode((uint16_t *)0x0000200D);  // 读取 x 轴编码器读数
    uint16_t u = HWReadEncode((uint16_t *)0x0000200E);  // 读取 u 轴编码器读数
    uint16_t w = HWReadEncode((uint16_t *)0x0000200F);  // 读取 w 轴编码器读数

    InitEncode(core->encode, AXIS_X, x);
    InitEncode(core->encode, AXIS_Y, y);
    InitEncode(core->encode, AXIS_Z, z);
    InitEncode(core->encode, AXIS_A, a);
    InitEncode(core->encode, AXIS_B, b);
    InitEncode(core->encode, AXIS_C, c);
    InitEncode(core->encode, AXIS_U, u);
    InitEncode(core->encode, AXIS_W, w);
}

void RobotCoreReadEncode(RobotCore* core)
{
    uint16_t x = HWReadEncode((uint16_t *)0x00002007);  // 读取 x 轴编码器读数
    uint16_t y = HWReadEncode((uint16_t *)0x00002009);  // 读取 y 轴编码器读数
    uint16_t z = HWReadEncode((uint16_t *)0x0000200A);  // 读取 z 轴编码器读数
    uint16_t a = HWReadEncode((uint16_t *)0x0000200B);  // 读取 a 轴编码器读数
    uint16_t b = HWReadEncode((uint16_t *)0x0000200C);  // 读取 b 轴编码器读数
    uint16_t c = HWReadEncode((uint16_t *)0x0000200D);  // 读取 x 轴编码器读数
    uint16_t u = HWReadEncode((uint16_t *)0x0000200E);  // 读取 u 轴编码器读数
    uint16_t w = HWReadEncode((uint16_t *)0x0000200F);  // 读取 w 轴编码器读数


    SetEncodeVal(core->encode, AXIS_X, x);
    SetEncodeVal(core->encode, AXIS_Y, y);
    SetEncodeVal(core->encode, AXIS_Z, z);
    SetEncodeVal(core->encode, AXIS_A, a);
    SetEncodeVal(core->encode, AXIS_B, b);
    SetEncodeVal(core->encode, AXIS_C, c);
    SetEncodeVal(core->encode, AXIS_U, u);
    SetEncodeVal(core->encode, AXIS_W, w);
}

void RobotCoreInit(RobotCore* core)
{
    core->softVerison = 1;
    core->hdVerison = 1;

    /* 初始化编码器 */
    RobotCoreInitEncode(core);
}

