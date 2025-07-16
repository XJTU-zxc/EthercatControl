#include "command.h"

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "params.h"
#include "compile.h"
#include "util.h"
#include "variabledef.h"
#include "public.h"
#include "motioncontrolparams.h"
#include "paramscpp.h"
#include "ProgrammerSystemParams.h"
#include "common.h"
#include "util.h"
#include "MyStack.h"

// void spin_lock(volatile unsigned int *lock) { while(__ldrex(lock) != 0 || __strex(1, lock) != 0); }

void delay2(int n)
{
    while (--n)
        ;
}
/* STM参数，急停参数初始化 */
int STM_EST_Lastlimit_ParamReset()
{
    g_params_stm = 0; // 0 不需要停；  1 所有轴全部停 ；  2 有个别轴要停
    int i;
    for (i = 0; i < 8; i++)
    {
        STMAxises[i] = 0;
    }
    /* 急停参数初始化 */
    g_params_est = 0;

    /* 插补运动上一次限位保存清零 */
    for (i = 0; i < 16; i++)
    {
        InterplastLimit[i] = 0;
    }

    return 0;
}

/**
 * @brief 用于参数是 nnnn 类型的指令的参数解析，
 * 例如如下指令：
 *  BGM ABCD;
 *  STM A;
 *
 * @return 0 :代表一个参数也没有，这不符合规定，所以返回值为 0 代表解析失败
 */
int16_t ParseParams1(Compiler *compiler, StringWrapper *wrapper)
{
    /* 参数列表，最低位为 1 代表存在参数 A，其它参数依次往上推 */
    int16_t mask = 0;
    int16_t bits = 0;

    /* 检测 tab 或者 空格 */
    int id = CompilerReadImportantToken(compiler, wrapper);

    /* 如果该指令后直接接 ; ，代表所有轴 */
    if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
    {
        return 0xff;
    }

    /**
     * @brief 标识符 A、B、C、D、E、F、G、H 在设计编号的时候可以连在一起，解析效率更高
     * 虽然是硬编码，但简单即好用
     */
    if (id < IDENTIFIER_A || id > IDENTIFIER_H)
    {
        return 0;
    }

    bits = (1 << (id - IDENTIFIER_A));
    mask |= bits;

    /* 继续检测后面的参数 */
    do
    {
        id = CompilerReadToken(compiler, wrapper);
        /* 检测到分号或者换行 */
        if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
        {
            break;
        }

        /* 不符合语法 */
        if (id < IDENTIFIER_A || id > IDENTIFIER_H)
        {
            return 0;
        }

        bits = (1 << (id - IDENTIFIER_A));

        /* 参数重复 */
        if ((mask & bits) != 0)
        {
            return 0;
        }

        mask |= bits;
    } while (1);

    return mask;
}

/**
 * @brief 用于参数是 m=n 类型的指令的参数解析，
 * 例如如下指令：
 *  DPC A=1000;
 *
 * @return 0 :代表一个参数也没有，这不符合规定，所以返回值为 0 代表解析失败
 */
int16_t ParseParams2(Compiler *compiler, StringWrapper *wrapper, int32_t *value)
{
    /**
     * @brief 该种参数有两种形式
     * 1 100,100,100,100
     * 2 A=100,B=100
     */

    /* 参数列表，最低位为 1 代表存在参数 A，其它参数依次往上推 */
    int16_t mask = 0;

    /* 当前解析到第 bits 个参数了 */
    int16_t bits = 0;

    /* 读取第一个参数 */
    int id = CompilerReadImportantToken(compiler, wrapper);

    /* 如果是第一种形式的参数 */
    if (id == IDENTIFIER_NUMBER)
    {
        /**
         * @brief 以第一种参数形式进行检测
         */

        /* 记录状态 */
        int rc = -1;

        /* 保存字符串转换成 int 的结果 */
        int32_t ii = -1;

        /* 将字符串转换为 int，转换的结果保存在 ii 中 */
        rc = String2Int(compiler->token, compiler->tokenlen, &ii);
        /* 转换失败，直接返回 */
        if (rc < 0)
        {
            return 0;
        }

        /* 第一个参数代表的是 A 轴，所以 mask 的第一位置为 1 */
        mask |= 0x01;
        value[0] = ii;
        ++bits;

        /* 继续解析后面的参数 */
        do
        {
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 如果是分号，跳出循环，检查剩下的参数 */
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                break;
            }

            /* 不是分号，就看是不是逗号，不是逗号直接返回 */
            if (id != IDENTIFIER_COMMA)
            {
                return 0;
            }

            /* 检查数字 */
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 不是数字，直接返回 */
            if (id != IDENTIFIER_NUMBER)
            {
                return 0;
            }

            /* 数字转换错误，直接返回 */
            rc = String2Int(compiler->token, compiler->tokenlen, &ii);
            if (rc < 0)
            {
                return 0;
            }

            /* 参数个数超过最大轴的个数，直接返回 */
            if (bits >= 8)
            {
                return 0;
            }

            /* 记录第 bits 个轴的参数 */
            mask |= (1 << bits);
            value[bits] = ii;
            ++bits;
        } while (1);
    }
    else if (id >= IDENTIFIER_A && id <= IDENTIFIER_H)
    {
        /**
         * @brief 以第二种参数形式进行检测
         */

        /* 记录状态 */
        int rc = -1;

        /* 保存字符串转换成 int 的结果 */
        int32_t ii = -1;

        /* 第 id - IDENTIFIER_A 个轴的参数 */
        bits = id - IDENTIFIER_A;

        /* 检测 = */
        id = CompilerReadImportantToken(compiler, wrapper);
        if (id != IDENTIFIER_EQUAL)
        {
            return 0;
        }

        /* 检测数字 */
        id = CompilerReadImportantToken(compiler, wrapper);
        if (id != IDENTIFIER_NUMBER)
        {
            return 0;
        }

        rc = String2Int(compiler->token, compiler->tokenlen, &ii);
        if (rc < 0)
        {
            return 0;
        }

        /* 第 id - IDENTIFIER_A 个轴的参数 */
        mask |= (1 << bits);
        value[bits] = ii;

        do
        {
            // 检测 ,或者;
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 如果是分号，这是一个完整的数据了 */
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                break;
            }
            /* 不是分号那就必须是 , 号 */
            if (id != IDENTIFIER_COMMA)
            {
                return 0;
            }

            // 检测 A-H
            id = CompilerReadImportantToken(compiler, wrapper);
            if (id < IDENTIFIER_A || id > IDENTIFIER_H)
            {
                return 0;
            }
            bits = id - IDENTIFIER_A;

            // 检测 =
            id = CompilerReadImportantToken(compiler, wrapper);
            if (id != IDENTIFIER_EQUAL)
            {
                return 0;
            }

            // 检测 数字
            id = CompilerReadImportantToken(compiler, wrapper);
            if (id != IDENTIFIER_NUMBER)
            {
                return 0;
            }

            rc = String2Int(compiler->token, compiler->tokenlen, &ii);
            if (rc < 0)
            {
                return 0;
            }

            /**
             * @brief 记录参数
             */

            /* 如果参数被重复记录，直接返回 */
            if ((mask & (1 << bits)) != 0)
            {
                return 0;
            }
            mask |= (1 << bits);
            value[bits] = ii;
        } while (1);
    }
    else
    {
        /* 参数错误 */
        return 0;
    }

    return mask;
}

/**
 * @brief 用于参数是 m=n 类型的指令的参数解析，n 是浮点型
 *
 * @return 0 :代表一个参数也没有，这不符合规定，所以返回值为 0 代表解析失败
 */
int16_t ParseParams3(Compiler *compiler, StringWrapper *wrapper, float *value)
{
    /**
     * @brief 该种参数有两种形式
     * 1 100,100,100,100
     * 2 A=100,B=100
     */

    /* 参数列表，最低位为 1 代表存在参数 A，其它参数依次往上推 */
    int16_t mask = 0;

    /* 当前解析到第 bits 个参数了 */
    int16_t bits = 0;

    /* 读取第一个参数 */
    int id = CompilerReadImportantToken(compiler, wrapper);

    /* 如果是第一种形式的参数 */
    if (id == IDENTIFIER_NUMBER || id == IDENTIFIER_FNUMBER)
    {
        /**
         * @brief 以第一种参数形式进行检测
         */

        /* 保存字符串转换成 int 的结果 */
        float ii = -1;

        /* 将字符串转换为 float，转换的结果保存在 ii 中 */
        ii = atof(compiler->token);

        /* 第一个参数代表的是 A 轴，所以 mask 的第一位置为 1 */
        mask |= 0x01;
        value[0] = ii;
        ++bits;

        /* 继续解析后面的参数 */
        do
        {
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 如果是分号，跳出循环，检查剩下的参数 */
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                break;
            }

            /* 不是分号，就看是不是逗号，不是逗号直接返回 */
            if (id != IDENTIFIER_COMMA)
            {
                return 0;
            }

            /* 检查数字 */
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 不是数字，直接返回 */
            if (id != IDENTIFIER_NUMBER && id != IDENTIFIER_FNUMBER)
            {
                return 0;
            }

            ii = atof(compiler->token);
            /* 参数个数超过最大轴的个数，直接返回 */
            if (bits >= 8)
            {
                return 0;
            }

            /* 记录第 bits 个轴的参数 */
            mask |= (1 << bits);
            value[bits] = ii;
            ++bits;
        } while (1);
    }
    else if (id >= IDENTIFIER_A && id <= IDENTIFIER_H)
    {
        /**
         * @brief 以第二种参数形式进行检测
         */
        /* 保存字符串转换成 float 的结果 */
        float ii = -1;

        /* 第 id - IDENTIFIER_A 个轴的参数 */
        bits = id - IDENTIFIER_A;

        /* 检测 = */
        id = CompilerReadImportantToken(compiler, wrapper);
        if (id != IDENTIFIER_EQUAL)
        {
            return 0;
        }

        /* 检测数字 */
        id = CompilerReadImportantToken(compiler, wrapper);
        if (id != IDENTIFIER_NUMBER && id != IDENTIFIER_FNUMBER)
        {
            return 0;
        }

        ii = atof(compiler->token);
        /* 第 id - IDENTIFIER_A 个轴的参数 */
        mask |= (1 << bits);
        value[bits] = ii;

        do
        {
            // 检测 ,或者;
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 如果是分号，这是一个完整的数据了 */
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                break;
            }
            /* 不是分号那就必须是 , 号 */
            if (id != IDENTIFIER_COMMA)
            {
                return 0;
            }

            // 检测 A-H
            id = CompilerReadImportantToken(compiler, wrapper);
            if (id < IDENTIFIER_A || id > IDENTIFIER_H)
            {
                return 0;
            }
            bits = id - IDENTIFIER_A;

            // 检测 =
            id = CompilerReadImportantToken(compiler, wrapper);
            if (id != IDENTIFIER_EQUAL)
            {
                return 0;
            }

            // 检测 数字
            id = CompilerReadImportantToken(compiler, wrapper);
            if (id != IDENTIFIER_NUMBER && id != IDENTIFIER_FNUMBER)
            {
                return 0;
            }

            ii = atof(compiler->token);
            /**
             * @brief 记录参数
             */

            /* 如果参数被重复记录，直接返回 */
            if ((mask & (1 << bits)) != 0)
            {
                return 0;
            }
            mask |= (1 << bits);
            value[bits] = ii;
        } while (1);
    }
    else
    {
        /* 参数错误 */
        return 0;
    }

    return mask;
}

/**
 * @brief 用于参数是 m,m,,,m 类型的指令的参数解析，n 是整型
 *
 * @return 0 :代表一个参数也没有，这不符合规定，所以返回值为 0 代表解析失败
 */
int16_t ParseParams4(Compiler *compiler, StringWrapper *wrapper, int32_t *value)
{
    int16_t mask = 0;
    /* 轴计数 */
    int axisCount = 0;
    /* 运行状态 */
    int rc = 0;
    /**
     * @brief 例如指令格式为 EGR ?,?
     */
    do
    {
        /* 检查参数数字 */
        int id = CompilerReadImportantToken(compiler, wrapper);

        /* 检查 ; 或者换行 */
        if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
        {
            break;
        }

        if (id != IDENTIFIER_NUMBER &&
            id != IDENTIFIER_COMMA)
        {
            return 0;
        }

        if (id == IDENTIFIER_NUMBER)
        {
            if (axisCount >= 8)
            {
                return 0;
            }

            rc = String2Int(compiler->token, compiler->tokenlen, &value[axisCount]);
            if (rc < 0)
            {
                return 0;
            }
            mask |= (1 << axisCount);

            /* 如果是数字，读取结束符或者 , */
            id = CompilerReadImportantToken(compiler, wrapper);
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                break;
            }

            if (id != IDENTIFIER_COMMA)
            {
                return 0;
            }
        }

        ++axisCount;
    } while (1);

    return mask;
}

/**
 * @brief 用于不带参数的指令的解析。例如指令 RPB;
 *
 * @return 0 :成功
 * @return -1 :失败
 */
int16_t ParseParams5(Compiler *compiler, StringWrapper *wrapper)
{
    int id = CompilerReadImportantToken(compiler, wrapper);
    /* 检查分号或者换行 */
    if (id != IDENTIFIER_SEMI && id != IDENTIFIER_NEWLINE1 && id != IDENTIFIER_NEWLINE2)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief 用于只带一个数字参数的指令的解析。例如 SET 10;
 *
 * @return 0 :成功
 * @return -1 :失败
 */
int16_t ParseParams6(Compiler *compiler, StringWrapper *wrapper, int32_t *value)
{
    /* 读取数字 */
    int id = CompilerReadImportantToken(compiler, wrapper);
    if (id != IDENTIFIER_NUMBER)
    {
        /* 不是数字返回 -1 */
        return -1;
    }

    /* 输入点序号 */
    int rc = String2Int(compiler->token, compiler->tokenlen, value);
    if (rc < 0)
    {
        return -1;
    }

    /* 检查分号或者换行 */
    id = CompilerReadImportantToken(compiler, wrapper);
    if (id != IDENTIFIER_SEMI && id != IDENTIFIER_NEWLINE1 && id != IDENTIFIER_NEWLINE2)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief 用于参数是 nnnn 类型的指令的参数解析，该参数带顺序
 * 例如如下指令：
 *  VIC BCA
 *
 * @return 0 :代表一个参数也没有，这不符合规定，所以返回值为 0 代表解析失败
 */
int16_t ParseParams7(Compiler *compiler, StringWrapper *wrapper, int *order, int *nbOrder)
{
    /* 参数列表，最低位为 1 代表存在参数 A，其它参数依次往上推 */
    int16_t mask = 0;
    int16_t bits = 0;
    int16_t order_i = 0;

    /* 检测 tab 或者 空格 */
    int id = CompilerReadImportantToken(compiler, wrapper);

    /**
     * @brief 标识符 A、B、C、D、E、F、G、H 在设计编号的时候可以连在一起，解析效率更高
     * 虽然是硬编码，但简单即好用
     */
    if (id < IDENTIFIER_A || id > IDENTIFIER_H)
    {
        return 0;
    }

    bits = (1 << (id - IDENTIFIER_A));
    mask |= bits;
    order[order_i++] = id - IDENTIFIER_A;

    /* 继续检测后面的参数 */
    do
    {
        id = CompilerReadToken(compiler, wrapper);
        /* 检测到分号或者换行 */
        if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
        {
            break;
        }

        /* 不符合语法 */
        if (id < IDENTIFIER_A || id > IDENTIFIER_H)
        {
            return 0;
        }

        bits = (1 << (id - IDENTIFIER_A));
        order[order_i++] = id - IDENTIFIER_A;

        /* 参数重复 */
        if ((mask & bits) != 0)
        {
            return 0;
        }

        mask |= bits;
    } while (1);

    *nbOrder = order_i;
    return mask;
}

/**
 * @brief 用于 VCW、VAW、IPA 等参数的解析，参数类型位固定的四个整型数据
 *
 * @return 0  : ok
 * @return -1 : fail
 */
int16_t ParseParams8(Compiler *compiler, StringWrapper *wrapper, int32_t *value)
{
    /* 当前解析到第 bits 个参数了 */
    int8_t bits = 0;

    /* 读取第一个参数 */
    int id = CompilerReadImportantToken(compiler, wrapper);

    /* 如果是第一种形式的参数 */
    if (id == IDENTIFIER_NUMBER)
    {
        /**
         * @brief 以第一种参数形式进行检测
         */

        /* 记录状态 */
        int rc = -1;

        /* 保存字符串转换成 int 的结果 */
        int32_t ii = -1;

        /* 将字符串转换为 int，转换的结果保存在 ii 中 */
        rc = String2Int(compiler->token, compiler->tokenlen, &ii);
        /* 转换失败，直接返回 */
        if (rc == -1)
        {
            return 0;
        }
        value[0] = ii;
        ++bits;

        /* 继续解析后面的参数 */
        do
        {
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 如果是分号，跳出循环，检查剩下的参数 */
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                break;
            }

            /* 不是分号，就看是不是逗号，不是逗号直接返回 */
            if (id != IDENTIFIER_COMMA)
            {
                return -1;
            }

            /* 检查数字 */
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 不是数字，直接返回 */
            if (id != IDENTIFIER_NUMBER)
            {
                return -1;
            }

            /* 数字转换错误，直接返回 */
            rc = String2Int(compiler->token, compiler->tokenlen, &ii);
            if (rc < 0)
            {
                return -1;
            }

            /* 参数个数超过最大参数个数，直接返回 */
            if (bits >= 4)
            {
                return -1;
            }

            /* 记录第 bits 个参数 */
            value[bits] = ii;
            ++bits;
        } while (1);
    }
    else
    {
        /* 参数错误 */
        return -1;
    }

    if (bits != 4)
    {
        return -1;
    }

    return 0;
}

/***
 * @brief 用于解析参数m,m,m,m,, ，其中 m 是 A-H轴
 * @return -1 失败
 * @return 0  成功
 */
int8_t ParseParams9(Compiler *compiler, StringWrapper *wrapper, int16_t *value)
{
    int axisCount = 0;
    int id = -1;

    do
    {
        /* 读取一个关键字 */
        id = CompilerReadImportantToken(compiler, wrapper);
        /* 如果是分号，跳出循环，检查剩下的参数 */
        if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
        {
            // if (lastSymbolIsComma == 1) {
            //     return -1;
            // }
            break;
        }
        /* 如果不是A-H或者逗号，失败 */
        if ((id < IDENTIFIER_A || id > IDENTIFIER_H) &&
            id != IDENTIFIER_COMMA)
        {
            return -1;
        }
        /* 最多 8 个轴 */
        if (axisCount >= 8)
        {
            return -1;
        }
        /* 记录轴数据 */
        if (id != IDENTIFIER_COMMA)
        {
            value[axisCount] = id - IDENTIFIER_A;
            id = CompilerReadImportantToken(compiler, wrapper);
            /**
             * @brief 检测一个逗号或者行结束符号
             */

            /* 如果是分号，跳出循环，检查剩下的参数 */
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                ++axisCount;
                break;
            }
            else if (id == IDENTIFIER_COMMA)
            {
                /* 如果是最后一个轴，不能加逗号 */
                if (axisCount == 7)
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        }

        ++axisCount;
    } while (1);

    if (axisCount == 0)
    {
        /* 一个参数都没有，报错 */
        return -1;
    }

    return 0;
}

/**
 * @brief 用于参数是m， n 类型的指令的参数解析，m，n 与都代表了计算正切轴切线跟随相关参数，与A-H轴无关，m,n是浮点型wyf
 * 用于VTN
 * @return 0 :代表解析成功
 * @return -1:代表参数个数不符合
 */
int16_t ParseParams10(Compiler *compiler, StringWrapper *wrapper, double *value)
{
    int count = 0;
    int id = CompilerReadImportantToken(compiler, wrapper);

    /* 如果是第一种形式的参数 */
    if (id == IDENTIFIER_NUMBER || id == IDENTIFIER_FNUMBER)
    {
        /**
         * @brief 以第一种参数形式进行检测
         */

        /* 保存字符串转换成 int 的结果 */
        double ii = -1;

        /* 将字符串转换为 float，转换的结果保存在 ii 中 */
        ii = atof(compiler->token);

        value[count] = ii;
        ++count;
        do
        {
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 如果是分号，跳出循环，检查剩下的参数 */
            if (id == IDENTIFIER_SEMI || id == IDENTIFIER_NEWLINE1 || id == IDENTIFIER_NEWLINE2)
            {
                break;
            }

            /* 不是分号，就看是不是逗号，不是逗号直接返回 */
            if (id != IDENTIFIER_COMMA)
            {
                return -1;
            }

            /* 检查数字 */
            id = CompilerReadImportantToken(compiler, wrapper);

            /* 不是数字，直接返回 */
            if (id != IDENTIFIER_NUMBER && id != IDENTIFIER_FNUMBER)
            {
                return -1;
            }

            ii = atof(compiler->token);

            value[count] = ii;
            ++count;
        } while (1);
    }
    else
    {
        return -1;
    }

    if (count != 2)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief VSR 的指令格式为 VSR s。以分号或者空格
 * */
int Command_VSR(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    const char *p = NULL;
    /* 跳过空格寻找第一个参数 */
    float vsr_t = atof(wrapper->buf);
    if (vsr_t > 10 || vsr_t < 0)
    {
        /* VSR 参数的范围不在 0-10 之间，是错误的 */
        /* 往 g_message_cpu2arm 里面塞 fail 消息。封装成 resp 格式 */

        if (isMdi)
        {
            /* 如果是 MDI 模式，返回数据给控制器 */
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_VSR;

        return TEC_ERROR_VSR;
    }
    /* 更新 VSR 指令的全局参数 */
    g_params_vsr = vsr_t;

    /**
     * 返回消息。往 g_message_cpu2arm 里面塞 ok 消息。封装成 resp 格式
     * */
    if (isMdi)
    {
        /* 如果是 MDI 模式，返回数据 */
        CStringAddRespOk(g_message_cpu2arm);
    }

    /* 检查后续的 ; 或者 换行 */
    p = strchr(wrapper->buf, ';');
    if (p != NULL)
    {
        wrapper->len -= (p - wrapper->buf + 1);
        wrapper->buf = p + 1;
        return 0;
    }

    /* 说明是以换行结尾的，直接返回，该行执行结束 */
    wrapper->len = 0;

    return 0;
}

int Command_TEC(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    const char *p = NULL;
    /**
     * 返回消息。往 g_message_cpu2arm 里面塞 TEC 消息。封装成 resp 格式
     * */
    if (isMdi)
    {
        CStringAddRespNumber(g_message_cpu2arm, g_params_tec);
    }

    /* 重新设置 TEC 代码 */
    g_params_tec = TEC_ERROR_NONE;

    /* 检查 ; */
    p = strchr(wrapper->buf, ';');
    if (p != NULL)
    {
        wrapper->len -= (p - wrapper->buf + 1);
        wrapper->buf = p + 1;
        return 0;
    }
    /* 说明是以换行结尾的，直接返回，该行执行结束 */
    wrapper->len = 0;

    return 0;
}

int Command_MIT(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    float value[8] = {0};
    int i = 0;

    mask = ParseParams3(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        // 执行失败
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }

    /* 对参数进行检查 */
    for (i = 0; i < 8; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            /* 如果这个轴有值 */
            if (value[i] < 0.004 || value[i] > 1.0)
            {
                // 执行失败
                if (isMdi)
                {
                    CStringAddRespFail(g_message_cpu2arm);
                }

                /* 记录错误信息 */
                g_params_tec = TEC_ERROR_MIT;
                return TEC_ERROR_MIT;
            }
        }
    }

    /* 所有的参数都检查通过了，才会赋值 */
    for (i = 0; i < 8; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            /* 如果这个轴有值 */
            g_params_mit[i] = value[i];
        }
    }

    /**
     * 返回消息。往 g_message_cpu2arm 里面塞 ok 消息。封装成 resp 格式
     * */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_MRT(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 读取 string */
    int id = CompilerReadImportantToken(compiler, wrapper);
    /* 将信息保存到 g_message_cpu2arm 中 */
    if (isMdi)
    {
        CStringAddRespString(g_message_cpu2arm, compiler->token, compiler->tokenlen);
    }

    /* 读取 ; 号或者换行 */
    id = CompilerReadImportantToken(compiler, wrapper);

    return 0;
}

int Command__FLS(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    // int16_t mask = 0;
    // int i = 0;
    // int nbAxis = 0;
    // char buf[80] = {0};
    // int blen = 0;
    // mask = ParseParams1(compiler, wrapper);

    // buf[0] = '*';
    // buf[1] = '?';
    // buf[2] = '\r';
    // buf[3] = '\n';
    // blen = 4;

    // /* 将 mask 加入 buf 中 */
    // buf[blen++] = ':';
    // // ltoa(mask, buf + blen, 10);
    // sprintf(buf + blen, "%ld", mask);
    // blen += strlen(buf + blen);
    // buf[blen++] = '\r';
    // buf[blen++] = '\n';

    // /* 根据 mask 的值读取 io 的值 */
    // for (i=0; i<8; ++i) {
    //     int16_t bits = (mask & (1 << i));
    //     if (bits != 0) {
    //         /* 如果这个轴有值，加入返回消息中 */
    //         buf[blen++] = ':';
    //         if ((g_io_plimitIO & (1<<(i+8))) > 0) {
    //             /* 当前 io 为 1 */
    //             buf[blen++] = '1';
    //         }
    //         else {
    //             /* 当前 io 为 0 */
    //             buf[blen++] = '0';
    //         }
    //         buf[blen++] = '\r';
    //         buf[blen++] = '\n';
    //         /* 轴数+1 */
    //         ++nbAxis;
    //     }
    // }

    // /* 数组的实际大小 */
    // buf[1] = '0' + nbAxis + 1;

    // /* 将 buf 中的信息复制到发送缓存区中 */
    // if (isMdi) {
    //     if (CStringCat(g_message_cpu2arm, buf, blen) < 0) {
    //         return TEC_ERROR_GENERAL;
    //     }
    // }

    return 0;
}

int Command__RLS(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    // int16_t mask = 0;
    // int i = 0;
    // int nbAxis = 0;
    // char buf[80] = {0};
    // int blen = 0;
    // mask = ParseParams1(compiler, wrapper);

    // buf[0] = '*';
    // buf[1] = '?';
    // buf[2] = '\r';
    // buf[3] = '\n';
    // blen = 4;

    // /* 将 mask 加入 buf 中 */
    // buf[blen++] = ':';
    // // ltoa(mask, buf + blen, 10);
    // sprintf(buf + blen, "%ld", mask);
    // blen += strlen(buf + blen);
    // buf[blen++] = '\r';
    // buf[blen++] = '\n';

    // /* 根据 mask 的值读取 io 的值 */
    // for (i=0; i<8; ++i) {
    //     int16_t bits = (mask & (1 << i));
    //     if (bits != 0) {
    //         /* 如果这个轴有值，加入返回消息中 */
    //         buf[blen++] = ':';
    //         if ((g_io_nlimitIO & (1<<i)) > 0) {
    //             /* 当前 io 为 1 */
    //             buf[blen++] = '1';
    //         }
    //         else {
    //             /* 当前 io 为 0 */
    //             buf[blen++] = '0';
    //         }
    //         buf[blen++] = '\r';
    //         buf[blen++] = '\n';
    //         /* 轴数+1 */
    //         ++nbAxis;
    //     }
    // }

    // /* 数组的实际大小 */
    // buf[1] = '0' + nbAxis + 1;

    // /* 将 buf 中的信息复制到发送缓存区中 */
    // if (isMdi) {
    //     if (CStringCat(g_message_cpu2arm, buf, blen) < 0) {
    //         return TEC_ERROR_GENERAL;
    //     }
    // }

    return 0;
}

int Command_BGM(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask = 0;
    int i = 0;
    int motionMode = 0;

    StringWrapper copy_wrapper;
    copy_wrapper.buf = wrapper->buf;
    copy_wrapper.len = wrapper->len;

    STM_EST_Lastlimit_ParamReset();

    int id = CompilerReadImportantToken(compiler, &copy_wrapper);
    if (id == IDENTIFIER_S)
    {
        int rc = ParseParams5(compiler, &copy_wrapper);
        if (rc < 0)
        {
            /* 参数错误 */
            if (isMdi)
            {
                CStringAddRespFail(g_message_cpu2arm);
            }

            /* 记录错误信息 */
            g_params_tec = TEC_ERROR_PROGRAM;

            return TEC_ERROR_PROGRAM;
        }
        wrapper->len = 0; /* 消耗字符串 */
        motionMode = 1;   /* 进入插补模式 */
    }
    else
    {
        mask = ParseParams1(compiler, wrapper);
        /* 参数解析失败 */
        if (mask == 0)
        {
            if (isMdi)
            {
                CStringAddRespFail(g_message_cpu2arm);
            }

            /* 记录错误信息 */
            g_params_tec = TEC_ERROR_BGM;

            return TEC_ERROR_BGM;
        }

        motionMode = 0;
    }
    printf("[INFO] motionMode = %d\n", motionMode);
    /* 使能点位运动 */
    if (motionMode == 0)
    {
        if (g_params_motion_mode == 1)
        {
            /* 如果进入了插补模式，如果当前正在进行插补运动，报错 */
            if (g_params_interp_is_running > 0)
            {
                if (isMdi)
                {
                    CStringAddRespFail(g_message_cpu2arm);
                }

                /* 记录错误信息 */
                g_params_tec = TEC_ERROR_BGM;
                return TEC_ERROR_BGM;
            }
        }
        /* 普通模式下直接执行 */
        if (g_axis_is_running & mask != 0)
        {
            /* 如果当前使能的轴正在运动，也要报错 */
            if (isMdi)
            {
                CStringAddRespFail(g_message_cpu2arm);
            }

            /* 记录错误信息 */
            g_params_tec = TEC_ERROR_BGM;
            return TEC_ERROR_BGM;
        }
        /* 如果要运动的轴电机未使能，报错 */
        if ((~((mask & g_params_mof) ^ (~mask))) != 0)
        {
            if (isMdi)
            {
                CStringAddRespFail(g_message_cpu2arm);
            }
            /* 记录错误信息 */
            g_params_tec = TEC_ERROR_BGM;
            return TEC_ERROR_BGM;
        }
        /* 如果要使能的轴是从动轴，也会报错 */
        for (i = 0; i < 8; ++i)
        {
            int16_t bits = (mask & (1 << i));
            if (bits != 0)
            {
                if (g_params_egm[i] != -1)
                {
                    /* 往 g_message_cpu2arm 里面塞 fail 消息。封装成 resp 格式 */
                    if (isMdi)
                    {
                        CStringAddRespFail(g_message_cpu2arm);
                    }

                    /* 记录错误信息 */
                    g_params_tec = TEC_ERROR_BGM;
                    return TEC_ERROR_BGM;
                }
            }
        }

        /* BGM 参数设置为0 */
        g_params_bgm = 0;
        /* 修改参数 */
        g_params_bgm |= mask;
        printf("[INFO] g_params_bgm: %X\n", g_params_bgm);
    }
    else
    {                                                                                                /* 进入插补模式 */
        if (g_params_motion_mode == 1 ||                                                             /* 如果已经是插补模式，报错 */
            g_axis_is_running != 0 ||                                                                /* 如果轴正在进行运动，报错 */
            g_params_interp_axis_enable == 0 ||                                                      /* 还未使能插补轴 */
            (~((g_params_interp_axis_enable & g_params_mof) ^ (~g_params_interp_axis_enable))) != 0) /* 如果插补轴未使能，报错 */
        {
            if (isMdi)
            {
                CStringAddRespFail(g_message_cpu2arm);
            }

            /* 记录错误信息 */
            g_params_tec = TEC_ERROR_BGM;
            return TEC_ERROR_BGM;
        }
        /* 设置为插补模式 */
        g_params_motion_mode = 1;

        /* 给CPU2发送信号，准备发送插补数据过来 */
    }

    /**
     * 返回消息。往 g_message_cpu2arm 里面塞 ok 消息。封装成 resp 格式
     * */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/**
 * @brief 绝对位置
 * */
int Command_MVA(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    STM_EST_Lastlimit_ParamReset();

    if ((g_params_bgm & (1 << 8)) != 0)
    {
        /* 如果正在运动或者 BGS使能了，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVA;
        return TEC_ERROR_MVA;
    }

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if ((~((mask & g_params_mof) ^ (~mask))) != 0)
    {
        /* 要运动的轴未使能 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVA;
        return TEC_ERROR_MVA;
    }

    /* 记录要移动的距离 */
    for (int i = 0; i < SLAVE_NUM; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            g_axis_mvx_params[i].type = 1;
            g_axis_mvx_params[i].val = value[i];
            std::cout << "slave_params[" << i << "].targetPosition = " << value[i] << std::endl;
        }
    }
    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/**
 * @brief 相对位置
 * */
int Command_MVR(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    STM_EST_Lastlimit_ParamReset();

    if ((g_params_bgm & (1 << 8)) != 0)
    {
        /* 如果正在运动或者 BGS使能了，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVR;
        return TEC_ERROR_MVR;
    }

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if ((~((mask & g_params_mof) ^ (~mask))) != 0)
    {
        /* 要运动的轴未使能 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVR;
        return TEC_ERROR_MVR;
    }

    /* 记录要移动的距离 */
    for (int i = 0; i < SLAVE_NUM; ++i)

    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            g_axis_mvx_params[i].type = 2;
            g_axis_mvx_params[i].val = value[i];
        }
    }

    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_AIT(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int rc = -1;
    int32_t ii = 0;
    /* 读取数字 */
    CompilerReadImportantToken(compiler, wrapper);

    /* 输入点序号 */
    rc = String2Int(compiler->token, compiler->tokenlen, &ii);
    if (rc < 0 || abs(ii) > 32 || ii == 0)
    {
        /* 报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_AIT;
        return TEC_ERROR_AIT;
    }

    g_params_commandline_ait_seq = abs(ii) - 1;
    /* 设置等待输入点的电平 */
    if (ii > 0)
    {
        /* 等待变高 */
        g_params_commandline_ait_val = 1;
    }
    else
    {
        /* 等待变低 */
        g_params_commandline_ait_val = 0;
    }

    /* 读取换行或者分号 */
    CompilerReadImportantToken(compiler, wrapper);
    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_EGM(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 有几个逗号 */
    int16_t params_egm[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    int i = 0;

    /* 如果轴正在运动，不能设置电子齿轮 */
    if (g_axis_is_running > 0 || g_params_interp_is_running > 0)
    {
        /* 如果轴正在运动，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_EGM;
        return TEC_ERROR_EGM;
    }

    int rc = ParseParams9(compiler, wrapper, params_egm);
    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_EGM;
        return TEC_ERROR_EGM;
    }

    /* 检测是否指定了从动轴为主动轴 */
    for (i = 0; i < 8; ++i)
    {
        if (params_egm[i] != -1)
        {
            if (params_egm[params_egm[i]] != -1)
            {
                if (isMdi)
                {
                    CStringAddRespFail(g_message_cpu2arm);
                }

                /* 记录错误信息 */
                g_params_tec = TEC_ERROR_EGM;
                return TEC_ERROR_EGM;
            }
        }
    }

    /* 赋值 */
    memcpy(g_params_egm, params_egm, sizeof(params_egm));
    /* 使能电子齿轮功能 */
    g_params_egm_enable = 1;

    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_EGR(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    float value[8] = {0};
    int i = 0;
    int disable_count = 0;

    mask = ParseParams3(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }

    /* 对参数进行检查，参数大小不能大于 127 */
    for (i = 0; i < 8; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            /* 如果这个轴有值 */
            if (abs(value[i]) > 127)
            {
                // 执行失败
                if (isMdi)
                {
                    CStringAddRespFail(g_message_cpu2arm);
                }

                /* 记录错误信息 */
                g_params_tec = TEC_ERROR_EGR;
                return TEC_ERROR_EGR;
            }
        }
    }

    /* 所有的参数都检查通过了，才会赋值 */
    for (i = 0; i < 8; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            /* 如果这个轴有值 */
            if (abs(value[i]) < 0.00787)
            {
                g_params_egr[i] = 0;
            }
            else
            {
                g_params_egr[i] = value[i];
            }
        }
    }

    /* 继续检查 EGR 的参数，如果都是 0 ，就失能电子齿轮模式 */
    for (i = 0; i < 8; ++i)
    {
        if (abs(g_params_egr[i]) < 0.00787)
        {
            g_params_egm[i] = -1;
            ++disable_count;
        }
    }

    /* 如果所有的轴的值都为 0， 失能电子齿轮功能 */
    if (disable_count == 8)
    {
        g_params_egm_enable = 0;
        for (i = 0; i < 8; ++i)
        {
            g_params_egm[i] = -1;
        }
    }

    /**
     * 返回消息。往 g_message_cpu2arm 里面塞 ok 消息。封装成 resp 格式
     * */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_AMP(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask = 0;
    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }

    g_params_commandline_amp = mask;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_TCP(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask;
    int32_t array[9];
    int nbarray = 1;

    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }
    array[0] = mask;

    /* 根据 mask 的值读取指令位置 */
    while (nbarray < 9)
    {
        int16_t bits = (mask & (1 << (nbarray - 1)));
        if (bits != 0)
        {
            array[nbarray++] = *g_params_axisRealPositionPtr[nbarray - 1];
        }
    }

    if (isMdi)
    {
        CStringAddRespInt32Array(g_message_cpu2arm, array, nbarray);
    }

    return 0;
}

int Command_DPE(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0 || g_axis_is_running > 0)
    {
        /* 如果轴正在运动，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_DPE;
        return TEC_ERROR_DPE;
    }

    if (mask & 0x01)
    {
        robotCore.encode.x.curr = value[0];
        robotCore.encode.x.last = value[0];
    }
    if (mask & 0x02)
    {
        robotCore.encode.y.curr = value[1];
        robotCore.encode.y.last = value[1];
    }
    if (mask & 0x04)
    {
        robotCore.encode.z.curr = value[2];
        robotCore.encode.z.last = value[2];
    }
    if (mask & 0x08)
    {
        robotCore.encode.a.curr = value[3];
        robotCore.encode.a.last = value[3];
    }
    if (mask & 0x10)
    {
        robotCore.encode.b.curr = value[4];
        robotCore.encode.b.last = value[4];
    }
    if (mask & 0x20)
    {
        robotCore.encode.c.curr = value[5];
        robotCore.encode.c.last = value[5];
    }
    if (mask & 0x40)
    {
        robotCore.encode.u.curr = value[6];
        robotCore.encode.u.last = value[6];
    }
    if (mask & 0x80)
    {
        robotCore.encode.w.curr = value[7];
        robotCore.encode.w.last = value[7];
    }

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_MON(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask = 0;
    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }

    g_params_mof |= mask;
    for (int i = 0; i < SLAVE_NUM; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            printf("[SET] slave[%d].enable = true\n", i);
        }
    }
    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_MOF(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask = 0;
    mask = ParseParams1(compiler, wrapper);

    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }

    /* 如果轴正在运动，报错 */
    if (g_axis_is_running != 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_GENERAL;
        return TEC_ERROR_GENERAL;
    }

    g_params_mof = g_params_mof ^ mask & g_params_mof;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_TCE(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask = 0;
    int32_t array[9];
    int nbarray = 1;

    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }

    array[0] = mask;
    if (mask & 0x01)
    {
        array[nbarray++] = robotCore.encode.x.curr;
    }
    if (mask & 0x02)
    {
        array[nbarray++] = robotCore.encode.y.curr;
    }
    if (mask & 0x04)
    {
        array[nbarray++] = robotCore.encode.z.curr;
    }
    if (mask & 0x08)
    {
        array[nbarray++] = robotCore.encode.a.curr;
    }
    if (mask & 0x10)
    {
        array[nbarray++] = robotCore.encode.b.curr;
    }
    if (mask & 0x20)
    {
        array[nbarray++] = robotCore.encode.c.curr;
    }
    if (mask & 0x40)
    {
        array[nbarray++] = robotCore.encode.u.curr;
    }
    if (mask & 0x80)
    {
        array[nbarray++] = robotCore.encode.w.curr;
    }

    /* 返回消息 */
    if (isMdi)
    {
        CStringAddRespInt32Array(g_message_cpu2arm, array, nbarray);
    }

    return 0;
}

int Command_VLI(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    isInterpProg = 1;
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    STM_EST_Lastlimit_ParamReset();

    mask = ParseParams4(compiler, wrapper, value);
    if (mask == 0)
    {
        /* 语法错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 如果未使能插补轴，报错 */
    if (~((mask & g_params_interp_axis_enable) ^ (~mask)) != 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        g_params_tec = TEC_ERROR_GENERAL;
        return TEC_ERROR_GENERAL;
    }

    /* 如果轴正在进行点位运动，报错 */
    /* 如果轴正在运动，报错 */
    if (g_axis_is_running != 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_GENERAL;
        return TEC_ERROR_GENERAL;
    }

    if (interpNum >= MaxInterpNum)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
            g_params_tec = TEC_ERROR_INTERP_FULL;
            return TEC_ERROR_INTERP_FULL;
        }
        isInterpProg = 2;
        return -1;
    }

    long double dist = 0;
    for (int i = 0; i < SLAVE_NUM; ++i)
    {
        dist += (long double)value[i] * (long double)value[i];
    }

    dist = sqrtl(dist);
    double k[8] = {0};
    for (int i = 0; i < SLAVE_NUM; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            k[i] = value[i] / dist;
        }
    }

    /* 参数赋值并发送给CPU2 */
    ipcLineData.vs = 0;
    ipcLineData.vm = g_params_vsp * g_params_vsr;
    ipcLineData.ve = 0;
    ipcLineData.am = g_params_vac * g_params_vac_coeff;
    ipcLineData.dist = dist;
    for (int i = 0; i < 8; i++)
    {
        ipcLineData.unitVec[i] = k[i];
    }
    interpNum++;
    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_DPC(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0 || g_axis_is_running > 0 || g_params_interp_is_running > 0)
    {
        /* 如果轴正在运动，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_DPC;
        return TEC_ERROR_DPC;
    }

    if (mask & 0x01)
    {
        PostionRegs.real_pos.pos_x = value[0];
        PostionRegs.AbsPos.pos_x = value[0];
        PostionRegs.real_posbk.pos_x = value[0];
        PostionRegs.last_real_posbk.pos_x = value[0];

        EncodeRegs.CsReg.X = value[0];
        EncodeRegs.BsReg.X = value[0];
        EncodeRegs.AsReg.X = value[0];
    }
    if (mask & 0x02)
    {
        PostionRegs.real_pos.pos_y = value[1];
        PostionRegs.AbsPos.pos_y = value[1];
        PostionRegs.real_posbk.pos_y = value[1];
        PostionRegs.last_real_posbk.pos_y = value[1];

        EncodeRegs.CsReg.Y = value[1];
        EncodeRegs.BsReg.Y = value[1];
        EncodeRegs.AsReg.Y = value[1];
    }
    if (mask & 0x04)
    {
        PostionRegs.real_pos.pos_z = value[2];
        PostionRegs.AbsPos.pos_z = value[2];
        PostionRegs.real_posbk.pos_z = value[2];
        PostionRegs.last_real_posbk.pos_z = value[2];

        EncodeRegs.CsReg.Z = value[2];
        EncodeRegs.BsReg.Z = value[2];
        EncodeRegs.AsReg.Z = value[2];
    }
    if (mask & 0x08)
    {
        PostionRegs.real_pos.pos_a = value[3];
        PostionRegs.AbsPos.pos_a = value[3];
        PostionRegs.real_posbk.pos_a = value[3];
        PostionRegs.last_real_posbk.pos_a = value[3];

        EncodeRegs.CsReg.A = value[3];
        EncodeRegs.BsReg.A = value[3];
        EncodeRegs.AsReg.A = value[3];
    }
    if (mask & 0x10)
    {
        PostionRegs.real_pos.pos_b = value[4];
        PostionRegs.AbsPos.pos_b = value[4];
        PostionRegs.real_posbk.pos_b = value[4];
        PostionRegs.last_real_posbk.pos_b = value[4];

        EncodeRegs.CsReg.B = value[4];
        EncodeRegs.BsReg.B = value[4];
        EncodeRegs.AsReg.B = value[4];
    }
    if (mask & 0x20)
    {
        PostionRegs.real_pos.pos_c = value[5];
        PostionRegs.AbsPos.pos_c = value[5];
        PostionRegs.real_posbk.pos_c = value[5];
        PostionRegs.last_real_posbk.pos_c = value[5];

        EncodeRegs.CsReg.C = value[5];
        EncodeRegs.BsReg.C = value[5];
        EncodeRegs.AsReg.C = value[5];
    }
    if (mask & 0x40)
    {
        PostionRegs.real_pos.pos_w = value[6];
        PostionRegs.AbsPos.pos_w = value[6];
        PostionRegs.real_posbk.pos_w = value[6];
        PostionRegs.last_real_posbk.pos_w = value[6];

        EncodeRegs.CsReg.W = value[6];
        EncodeRegs.BsReg.W = value[6];
        EncodeRegs.AsReg.W = value[6];
    }
    if (mask & 0x80)
    {
        PostionRegs.real_pos.pos_u = value[7];
        PostionRegs.AbsPos.pos_u = value[7];
        PostionRegs.real_posbk.pos_u = value[7];
        PostionRegs.last_real_posbk.pos_u = value[7];

        EncodeRegs.CsReg.U = value[7];
        EncodeRegs.BsReg.U = value[7];
        EncodeRegs.AsReg.U = value[7];
    }

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/**
 * @brief 设置点位运动速度
 */
int Command_MSP(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MSP;
        return TEC_ERROR_MSP;
    }
    for (int i = 0; i < SLAVE_NUM; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[i] = (float)value[i] / 9155 / 10000 * 60;
            bpmParams.msp[i] = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[i];
            bpmParams.HasMspParam[i] = 1;
        }
    }

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_MAC(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        /* 如果轴正在运动，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MAC;
        return TEC_ERROR_MAC;
    }
    for (int i = 0; i < SLAVE_NUM; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[i] = (float)value[i] / 9155 / 10000 * 60;
            bpmParams.mac[i] = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[i];
            bpmParams.HasMacParam[i] = 1;
        }
    }

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_CLB(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int rc = ParseParams5(compiler, wrapper);

    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_CLB;
        return TEC_ERROR_CLB;
    }

    /* 清除程序运行相关标志位 */
    g_params_is_execute_prog = 0;
    g_params_prog_counter = 0;

    g_params_wtm_count = 0;            // 暂停时间等待位
    g_params_commandline_ait_seq = -1; // IO等待位
    g_params_commandline_amp = 0;      // 点位运动等待位

    /*  清除程序缓冲区 */
    g_programmerBulk.Clear();
    /* 给CPU2发送信号，停止发送插补数据过来 */
    // CPU1SendMessageToCPU2(3,1);
    interpNum = 0;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_RPB(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);

    if (rc < 0 || (ii != 0 && ii != 1))
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 添加返回消息 */
    if (isMdi)
    {
        if (ii == 0)
        {
            CStringAddRespNumber(g_message_cpu2arm, PROG_MAX_LINE_SIZE - g_programmerBulk.Size());
        }
        else if (ii == 1)
        {
            CStringAddRespNumber(g_message_cpu2arm, interpNum);
        }
    }

    return 0;
}

int Command_SET(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii > 32 || ii <= 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    // if(!IPC_isFlagBusyLtoR(IPC_CPU1_L_CM_R, IPC_FLAG7))
    // {
    //     uint16_t temp = ii;
    //     uint16_t flag = 0x1;
    //     if(temp >= 1 && temp <= 16)
    //         ThreeWordsReg.IOLow |= (flag << (temp-1));
    //     else if(temp > 16 && temp <= 32)
    //         ThreeWordsReg.IOHigh |= (flag << (temp - 17));

        // CToMRegs.IOdata[0] = ThreeWordsReg.IOLow;
        // CToMRegs.IOdata[1] = ThreeWordsReg.IOHigh;

    //     IPC_sendCommand(IPC_CPU1_L_CM_R, IPC_FLAG7, IPC_ADDR_CORRECTION_ENABLE, 1, 0, 2);
    //     IPC_setFlagLtoR(IPC_CPU1_L_CM_R, IPC_FLAG7);
    // } else {
    //     /* IPC 繁忙 */
    //     if (isMdi) {
    //         CStringAddRespFail(g_message_cpu2arm);
    //     }

    //     /* 记录错误信息 */
    //     g_params_tec = TEC_ERROR_SET_BUSY;
    //     return TEC_ERROR_SET_BUSY;
    // }

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_WTM(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);

    if (ii < 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_GENERAL;
        return TEC_ERROR_GENERAL;
    }

    g_params_wtm_count = ii * 20;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_TER(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask;
    int32_t array[9];
    int nbarray = 1;

    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        return TEC_ERROR_PROGRAM;
    }
    array[0] = mask;
    /* 根据 mask 的值读取指令位置 */
    if (mask & 0x01)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_x - robotCore.encode.x.curr;
    }
    if (mask & 0x02)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_y - robotCore.encode.y.curr;
    }
    if (mask & 0x04)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_z - robotCore.encode.z.curr;
    }
    if (mask & 0x08)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_a - robotCore.encode.a.curr;
    }
    if (mask & 0x10)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_b - robotCore.encode.b.curr;
    }
    if (mask & 0x20)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_c - robotCore.encode.c.curr;
    }
    if (mask & 0x40)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_w - robotCore.encode.u.curr;
    }
    if (mask & 0x80)
    {
        array[nbarray++] = PostionRegs.real_pos.pos_u - robotCore.encode.w.curr;
    }

    if (isMdi)
    {
        CStringAddRespInt32Array(g_message_cpu2arm, array, nbarray);
    }

    return 0;
}

int Command_TCV(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int16_t mask;
    int32_t array[9];
    int nbarray = 1;

    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        return TEC_ERROR_PROGRAM;
    }
    array[0] = mask;
    /* 根据 mask 的值读取指令位置 */
    double coeff = 1000000 / (SpeedFilterQueue::SPEED_FILTER_BUFFER_SIZE * 500); /* 换算系数，500 是 500 us 读一次编码器的值 */

    if (mask & 0x01)
    {
        array[nbarray++] = g_speedFilterQueueA.Get() * coeff;
    }
    if (mask & 0x02)
    {
        array[nbarray++] = g_speedFilterQueueB.Get() * coeff;
    }
    if (mask & 0x04)
    {
        array[nbarray++] = g_speedFilterQueueC.Get() * coeff;
    }
    if (mask & 0x08)
    {
        array[nbarray++] = g_speedFilterQueueD.Get() * coeff;
    }
    if (mask & 0x10)
    {
        array[nbarray++] = g_speedFilterQueueE.Get() * coeff;
    }
    if (mask & 0x20)
    {
        array[nbarray++] = g_speedFilterQueueF.Get() * coeff;
    }
    if (mask & 0x40)
    {
        array[nbarray++] = g_speedFilterQueueG.Get() * coeff;
    }
    if (mask & 0x80)
    {
        array[nbarray++] = g_speedFilterQueueH.Get() * coeff;
    }

    if (isMdi)
    {
        CStringAddRespInt32Array(g_message_cpu2arm, array, nbarray);
    }

    return 0;
}

int Command_SFL(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    int i = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        return TEC_ERROR_PROGRAM;
    }

    /* 记录要移动的距离 */
    for (i = 0; i < 8; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            if (value[i] == 2147483647)
            {
                ParamterUnRegs.ParamterRegs.AxisparmRegs.PSoftLimit[i] = 400000000;
            }
            else
            {
                ParamterUnRegs.ParamterRegs.AxisparmRegs.PSoftLimit[i] = value[i];
            }
        }
    }

    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_SRL(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    int i = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        return TEC_ERROR_PROGRAM;
    }

    /* 记录要移动的距离 */
    for (i = 0; i < 8; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            if (value[i] == -2147483647)
            {
                ParamterUnRegs.ParamterRegs.AxisparmRegs.NSoftLimit[i] = 0;
            }
            else
            {
                ParamterUnRegs.ParamterRegs.AxisparmRegs.NSoftLimit[i] = value[i];
            }
        }
    }

    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_EPG(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);

    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if (ii == 0)
    {
        g_params_is_execute_prog = 1; // arm程序执行置位
    }
    else if (ii == 1)
    {
        // RunstatusRegs.codeFMHandle.readProgNameAddress = FLASH_PERSIST_PROGNUM_START;
        // g_params_is_execute_prog = 2;   // flash程序执行置位
        // /* 读取一个字节的程序名数量 */
        // readUint16(&subProgramNum,&RunstatusRegs.codeFMHandle.readProgNameAddress,1);
        // /* 读取两个字节的程序起始地址 */
        // readProgramAddr(&RunstatusRegs.codeFMHandle,&RunstatusRegs.codeFMHandle.readProgNameAddress,2);
        /* 栈初始化 */
        Init_Stack(mystack);
    }
    /* 运行程序变量置位 */
    progNumTest = 0;
    g_params_prog_counter = 0;         // 程序计数器置位
    g_params_wtm_count = 0;            // 暂停时间等待位
    g_params_commandline_ait_seq = -1; // IO等待位
    g_params_commandline_amp = 0;      // 点位运动等待位

    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_CLR(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii > 32 || ii <= 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    // if(!IPC_isFlagBusyLtoR(IPC_CPU1_L_CM_R, IPC_FLAG7))
    // {
    //     uint16_t temp = ii;
    //     uint16_t flag = 0x1;
    //     if(temp >= 1 && temp <= 16)
    //         ThreeWordsReg.IOLow &= ~(flag << (temp-1));
    //     else if(temp > 16 && temp <= 32)
    //         ThreeWordsReg.IOHigh &= ~(flag << (temp - 17));

    //     CToMRegs.IOdata[0] = ThreeWordsReg.IOLow;
    //     CToMRegs.IOdata[1] = ThreeWordsReg.IOHigh;

    //     IPC_sendCommand(IPC_CPU1_L_CM_R, IPC_FLAG7, IPC_ADDR_CORRECTION_ENABLE, 1, 0, 2);
    //     IPC_setFlagLtoR(IPC_CPU1_L_CM_R, IPC_FLAG7);
    // } else {
    //     /* IPC 繁忙 */
    //     if (isMdi) {
    //         CStringAddRespFail(g_message_cpu2arm);
    //     }

    //     /* 记录错误信息 */
    //     g_params_tec = TEC_ERROR_IO_BUSY;
    //     return TEC_ERROR_IO_BUSY;
    // }

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_STM(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    StringWrapper copy_wrapper;
    copy_wrapper.buf = wrapper->buf;
    copy_wrapper.len = wrapper->len;

    /* 检查是否存在参数 */
    int rc = ParseParams5(compiler, &copy_wrapper);
    /* 无参数，全部停止 */
    if (rc == 0)
    {
        /* 停止所有轴的运动和程序 */
        /* 停止所有程序 */
        g_params_is_execute_prog = 0; // 程序执行置位
        g_params_prog_counter = 0;    // 程序计数器置位
        g_params_wtm_count = 0;       // 等待时间置位
        Init_Stack(mystack);          // 重置栈
        /* 停止所有轴的运动 */
        g_params_stm = 1;
        int j;
        for (j = 0; j < 8; j++)
        {
            STMAxises[j] = 1;
        }

        g_axis_is_running = 0;
        g_params_interp_is_running = 0;
        wrapper->len = 0; /* 解析成功 */
        /* 给CPU2发送信号，停止发送插补数据过来 */
        // CPU1SendMessageToCPU2(3,1);
        interpNum = 0;
    }
    else
    {
        /*  */
        int16_t mask = 0;

        mask = ParseParams1(compiler, wrapper);
        /* 参数解析失败 */
        if (mask == 0)
        {
            CStringAddRespFail(g_message_cpu2arm);
            g_params_tec = TEC_ERROR_PROGRAM;
            return TEC_ERROR_PROGRAM;
        }

        g_params_stm = 2;
        /* 停止指定轴的运动 */
        if (mask & 0x01)
        {
            STMAxises[0] = 1;
        }
        if (mask & 0x02)
        {
            STMAxises[1] = 1;
        }
        if (mask & 0x04)
        {
            STMAxises[2] = 1;
        }
        if (mask & 0x08)
        {
            STMAxises[3] = 1;
        }
        if (mask & 0x10)
        {
            STMAxises[4] = 1;
        }
        if (mask & 0x20)
        {
            STMAxises[5] = 1;
        }
        if (mask & 0x40)
        {
            STMAxises[6] = 1;
        }
        if (mask & 0x80)
        {
            STMAxises[7] = 1;
        }
    }

    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/**
 * @brief 退出插补模式
 */
int Command_VIE(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int rc = ParseParams5(compiler, wrapper);

    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 如果轴正在运动，报错 */
    if (g_axis_is_running != 0 || g_params_interp_is_running != 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 清空插补轴 */
    g_params_interp_axis_enable = 0;
    /* 结束正切模式wyf */
    g_params_vtn_enable = 0;
    /* 结束插补模式 */
    g_params_motion_mode = 0;
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

    /* 给CPU2发送信号，停止发送插补数据过来 */
    // CPU1SendMessageToCPU2(3,1);
    interpNum = 0;
    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_VIC(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{

    // CStringAddRespOk(g_message_cpu2arm);
    // return 0;
    int16_t mask;
    int order[8] = {-1, -1, -1};
    int nbOrder = 0;

    mask = ParseParams7(compiler, wrapper, order, &nbOrder);
    /* 参数解析失败 */
    if (mask == 0) {
        if (isMdi) {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;

        return TEC_ERROR_PROGRAM;
    }

    /* 如果轴正在运动，报错 */
    if (g_axis_is_running != 0 || g_params_interp_is_running != 0) {
        if (isMdi) {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 如果已经设置了 VIC ，报错 */
    if (g_params_interp_axis_enable != 0) {
        if (isMdi) {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_GENERAL;
        return TEC_ERROR_GENERAL;
    }

    // /* 设置二维平面轴 */
    // if (nbOrder >= 2) {
    //     g_params_plate_interp_axis[0] = order[0];
    //     g_params_plate_interp_axis[1] = order[1];
    //     g_params_plate_interp_axis[2] = order[2];
    // }

    // /* 设置二维平面轴及正切轴wyf*/
    // if (nbOrder >= 2) {
    //     g_params_vtn_axis[0] = order[0];
    //     g_params_vtn_axis[1] = order[1];
    //     g_params_vtn_axis[2] = order[2];
    // }

    /* 使能插补轴 */
    g_params_interp_axis_enable = mask;

    /* 检测是否进行了三轴联动，只有三轴情况下才可以实现正切wyf */
    if (nbOrder == 3) {
        g_params_tangent_available = 1;
    }

    /* 添加返回消息 */
    if (isMdi) {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/* 逆时针圆弧 */
int Command_VAW(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    isInterpProg = 1;
    int32_t value[4];
    STM_EST_Lastlimit_ParamReset();
    /* 不解析 */
    int rc = ParseParams8(compiler, wrapper, value);
    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if (interpNum >= MaxInterpNum)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
            g_params_tec = TEC_ERROR_INTERP_FULL;
            return TEC_ERROR_INTERP_FULL;
        }
        isInterpProg = 2;
        return -1;
    }
    // wyf
    circleParam[2] = value[2];
    circleParam[3] = value[3];
    circleParam[4] = 3;

    ipcCircleData.vm = ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed;
    ipcCircleData.am = ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc;
    ipcCircleData.edx = value[0];
    ipcCircleData.edy = value[1];
    ipcCircleData.i = value[2];
    ipcCircleData.j = value[3];
    ipcCircleData.circle_type = 3;

    // for(int i = 0; i < sizeof(struct IpcCircleData)/sizeof(uint16_t); i++){
    //     CPU1Send[i] = ((uint16_t *)&ipcCircleData)[i];
    // }
    /* 给CPU2发送消息 */
    // CPU1SendMessageToCPU2(1,sizeof(struct IpcCircleData)/sizeof(uint16_t));
    // IPC_waitForAck(IPC_CPU1_L_CPU2_R, IPC_FLAG0);
    interpNum++;
    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/* 顺时针圆弧 */
int Command_VCW(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    isInterpProg = 1;
    int32_t value[4];
    STM_EST_Lastlimit_ParamReset();
    /* 不解析 */
    int rc = ParseParams8(compiler, wrapper, value);
    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if (interpNum >= MaxInterpNum)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
            g_params_tec = TEC_ERROR_INTERP_FULL;
            return TEC_ERROR_INTERP_FULL;
        }
        isInterpProg = 2;
        return -1;
    }
    // wyf
    circleParam[2] = value[2];
    circleParam[3] = value[3];
    circleParam[4] = 2;

    ipcCircleData.vm = ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed;
    ipcCircleData.am = ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc;
    ipcCircleData.edx = value[0];
    ipcCircleData.edy = value[1];
    ipcCircleData.i = value[2];
    ipcCircleData.j = value[3];
    ipcCircleData.circle_type = 2;

    // for(int i = 0; i < sizeof(struct IpcCircleData)/sizeof(uint16_t); i++){
    //     CPU1Send[i] = ((uint16_t *)&ipcCircleData)[i];
    // }
    /* 给CPU2发送消息 */
    // CPU1SendMessageToCPU2(1,sizeof(struct IpcCircleData)/sizeof(uint16_t));
    // IPC_waitForAck(IPC_CPU1_L_CPU2_R, IPC_FLAG0);
    interpNum++;
    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_VSP(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);

    if (ii < 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_GENERAL;
        return TEC_ERROR_GENERAL;
    }

    /* 直线插补速度 */
    g_params_vsp = ii * g_params_vsp_coeff;
    /* 圆弧插补速度 */
    ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed = (float)ii / 9155 / 10000 * 60;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_VAC(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);

    if (ii < 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_GENERAL;
        return TEC_ERROR_GENERAL;
    }
    /* 直线插补加速度 */
    g_params_vac = ii * g_params_vac_coeff;
    /* 圆弧插补加速度 */
    ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc = (float)ii / 9155 / 10000 * 60;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/* 第三轴正切轴控制wyf */
int Command_VTN(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    double value[2] = {0};

    if (g_params_tangent_available == 1)
    {
        int rc = ParseParams10(compiler, wrapper, value);

        if (rc < 0)
        {
            /* 参数解析错误 */
            if (isMdi)
            {
                CStringAddRespFail(g_message_cpu2arm);
            }

            /* 记录错误信息 */
            g_params_tec = TEC_ERROR_PROGRAM;
            return TEC_ERROR_PROGRAM;
        }
        g_params_vtn_enable = 1; // VTN使能
        g_params_vtn_puls_per_degree = value[0];
        g_params_vtn_init_pos = value[1];
        init_tan_K_flag = true;
        CStringAddRespOk(g_message_cpu2arm);
        return 0;
    }
    /* 未设定正切轴直接返回错误信息 */
    else
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
    }
    return 0;
}

int Command_TSV(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int rc = ParseParams5(compiler, wrapper);

    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 添加返回消息 */
    if (isMdi)
    {
        if (bpmParams.tsv[0] != 'P')
        {
            CStringAddRespNumber(g_message_cpu2arm, 1);
        }
        else
        {
            CStringAddRespString(g_message_cpu2arm, bpmParams.tsv, 10);
        }
    }

    return 0;
}

int Command_TFV(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int rc = ParseParams5(compiler, wrapper);

    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 添加返回消息 */
    if (isMdi)
    {
        if (bpmParams.tfv[0] != 'P')
        {
            CStringAddRespNumber(g_message_cpu2arm, 1);
        }
        else
        {
            CStringAddRespString(g_message_cpu2arm, bpmParams.tfv, 10);
        }
    }

    return 0;
}

/**急停**/
int Command_EST(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t value;
    int rc = ParseParams6(compiler, wrapper, &value);

    if (rc < 0 || (value != 0 && value != 1))
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }
    /* 指令位置停止更新 */
    g_params_est = 1;
    /* 如果value为0，需要停程序 */
    if (value == 0)
    {
        g_params_est = 2;
    }
    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }
    return 0;
}

int Command_RST(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t value;
    int rc = ParseParams6(compiler, wrapper, &value);

    if (rc < 0 || (value != 0 && value != 1))
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    // RST标志位置1，main函数循环检测是否需要复位
    RunstatusRegs.RSTFlag = 1;

    /* value为1时控制器恢复出厂设置 */
    if (value == 1)
    {
        /* 清空flash程序区域 */

        /* 清空flash参数区域 */
    }
    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

/*
 * @brief 手动模式
 * */
int Command_JOG(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    /* 参数位 */
    int16_t mask = 0;
    int i = 0;
    /* 保存解析的参数 */
    int32_t value[8] = {0};
    STM_EST_Lastlimit_ParamReset();

    mask = ParseParams4(compiler, wrapper, value);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if ((~((mask & g_params_mof) ^ (~mask))) != 0)
    {
        /* 要运动的轴未使能 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVA;
        return TEC_ERROR_MVA;
    }

    for (i = 0; i < SLAVE_NUM; i++)
    {
        if (abs(value[i]) > 100000)
        {
            if (isMdi)
            {
                CStringAddRespFail(g_message_cpu2arm);
            }
            g_params_tec = TEC_ERROR_PROGRAM;
            return TEC_ERROR_PROGRAM;
        }
    }
    /* 新的JOG命令 */
    NewJogCommand = 1;
    /* JOG指定轴置位 */
    jogAxis = 0;
    /* 记录需要jog的轴，并设置轴速度 */
    for (i = 0; i < 8; ++i)
    {
        int16_t bits = (mask & (1 << i));
        if (bits != 0)
        {
            if (value[i] > 0)
            {
                jogAxis = jogAxis | (1 << 2 * i);
            }
            else if (value[i] < 0)
            {
                jogAxis = jogAxis | (1 << 2 * i + 1);
            }
        }
    }
    for (i = 0; i < 8; ++i)
    {
        if (value[i] < 0)
        {
            value[i] = -value[i];
        }
    }

    /* 设定轴速度 */
    if (mask & 0x01)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[0] = (float)(value[0]) / 9155 / 10000 * 60;
    }
    if (mask & 0x02)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[1] = (float)(value[1]) / 9155 / 10000 * 60;
    }
    if (mask & 0x04)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[2] = (float)(value[2]) / 9155 / 10000 * 60;
    }
    if (mask & 0x08)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[3] = (float)(value[3]) / 9155 / 10000 * 60;
    }
    if (mask & 0x10)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[4] = (float)(value[4]) / 9155 / 10000 * 60;
    }
    if (mask & 0x20)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[5] = (float)(value[5]) / 9155 / 10000 * 60;
    }
    if (mask & 0x40)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[6] = (float)(value[6]) / 9155 / 10000 * 60;
    }
    if (mask & 0x80)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[7] = (float)(value[7]) / 9155 / 10000 * 60;
    }
    /* 使能jog,deal_4096()中判断 */
    g_params_jog_enable = 1;

    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_FSW(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi) // 寻找回零开关  zjq
{
    /* 参数位 */
    int16_t mask = 0;

    if ((g_params_bgm & (1 << 8)) != 0)
    {
        /* 如果正在运动或者 BGS使能了，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVR;
        return TEC_ERROR_MVR;
    }

    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if ((~((mask & g_params_mof) ^ (~mask))) != 0)
    {
        /* 要运动的轴未使能 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVR;
        return TEC_ERROR_MVR;
    }

    fsw_enable_flag |= mask;
    fsw_enable_flag_last_buf = fsw_enable_flag;

    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_FID(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi) // 寻找回零z脉冲  zjq
{
    /* 参数位 */
    int16_t mask = 0;
    int i = 0;

    if ((g_params_bgm & (1 << 8)) != 0)
    {
        /* 如果正在运动或者 BGS使能了，报错 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVR;
        return TEC_ERROR_MVR;
    }

    mask = ParseParams1(compiler, wrapper);
    /* 参数解析失败 */
    if (mask == 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if ((~((mask & g_params_mof) ^ (~mask))) != 0)
    {
        /* 要运动的轴未使能 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_MVR;
        return TEC_ERROR_MVR;
    }

    /* fid的轴和fsw不匹配 报错 */
    for (i = 0; i < 8; i++)
    {
        if ((mask & (i << i)) != 0 && (fsw_enable_flag_last_buf & (i << i)) == 0)
        {
            CStringAddRespFail(g_message_cpu2arm);
            g_params_tec = TEC_ERROR_FID;
            return TEC_ERROR_FID;
        }
    }

    fid_enable_flag |= mask;

    /* 返回 ok 信息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_RDO(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi) // IO输出 zjq
{
    int32_t ii;
    int inBitStatus = 0; // 记录该IO输出状态
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii > 33 || ii <= 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    uint16_t temp = ii;
    uint16_t flag = 0x1;
    if (temp != 33)
    {
        if (temp >= 1 && temp <= 16)
        {
            inBitStatus = (flag << (temp - 1)) & ThreeWordsReg.IOLow; // 读io低16位输出
        }

        else if (temp > 16 && temp <= 32)
        {

            inBitStatus = (flag << (temp - 1)) & ThreeWordsReg.IOHigh; // 读io低16位输出
        }
        // inBitStatus不为0说明有输出置1
        if (inBitStatus != 0)
        {
            inBitStatus = 1;
        }
        /* 返回 OK 消息 */
        if (isMdi)
        {
            CStringAddRespNumber(g_message_cpu2arm, inBitStatus);
        }
    }
    else
    {
        char buf[32] = {0};
        for (int k = 1; k <= 32; k++)
        {
            if (k <= 16)
            {
                if (((flag << (k - 1)) & ThreeWordsReg.IOLow) != 0)
                {
                    buf[k - 1] = '1';
                }
                else
                {
                    buf[k - 1] = '0';
                }
            }
            else
            {
                if (((flag << (k - 1)) & ThreeWordsReg.IOHigh) != 0)
                {
                    buf[k - 1] = '1';
                }
                else
                {
                    buf[k - 1] = '0';
                }
            }
        }

        if (isMdi)
        {
            CStringAddRespString(g_message_cpu2arm, buf, 32);
        }
    }

    return 0;
}

int Command_RDI(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi) // IO输入 zjq
{
    int32_t ii;
    int inBitStatus = 0; // 记录该IO输入状态
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii > 33 || ii <= 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    uint16_t temp = ii;
    uint16_t flag = 0x1;
    if (temp != 33)
    {
        if (temp >= 1 && temp <= 16)
        {

            inBitStatus = (flag << (temp - 1)) & ThreeWordsReg.iIOLow; // 读io低16位输入
        }

        else if (temp > 16 && temp <= 32)
        {

            inBitStatus = (flag << (temp - 1)) & ThreeWordsReg.iIOHigh; // 读io高16位输入
        }

        // inBitStatus不为0说明有输入置1
        if (inBitStatus != 0)
        {
            inBitStatus = 1;
        }

        /* 返回 OK 消息 */
        if (isMdi)
        {
            CStringAddRespNumber(g_message_cpu2arm, inBitStatus);
        }
    }
    else
    {
        char buf[32] = {0};
        for (int k = 1; k <= 32; k++)
        {
            if (k <= 16)
            {
                if (((flag << (k - 1)) & ThreeWordsReg.iIOLow) != 0)
                {
                    buf[k - 1] = '1';
                }
                else
                {
                    buf[k - 1] = '0';
                }
            }
            else
            {
                if (((flag << (k - 1)) & ThreeWordsReg.iIOHigh) != 0)
                {
                    buf[k - 1] = '1';
                }
                else
                {
                    buf[k - 1] = '0';
                }
            }
        }

        if (isMdi)
        {
            CStringAddRespString(g_message_cpu2arm, buf, 32);
        }
    }

    return 0;
}

int Command_BKC(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{

    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii == 2147483647 || ii == -2147483648)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }
    g_bkc_flag = 1;
    RCCompInit();

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_DEC(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || (ii != 0 && ii != 1))
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    g_dec_flag = ii; // 螺距补偿flag

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_DFN(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii >= 1000 || ii < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    g_dfn_len = ii;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_DRN(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii >= 1000 || ii < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    g_drn_len = ii;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_DZN(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii == 2147483647 || ii == -2147483648)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    //    g_dzn_len = ii;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_DIT(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t ii;
    int rc = ParseParams6(compiler, wrapper, &ii);
    if (rc < 0 || ii == 2147483647 || ii == -2147483648)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    g_dit_param = ii;

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }

    return 0;
}

int Command_BPM(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{

    int rc = ParseParams5(compiler, wrapper);

    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    rc = PersistBPM();
    if (rc < 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PERSIST;
        return TEC_ERROR_PERSIST;
    }

    BPMParams m;
    // memcpy(&m,(BPMParams *) FLASH_PERSIST_PARAM_ADDR_START, sizeof(BPMParams));

    /* 返回 OK 消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }
    return rc;
}

int Command_BSN(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{

    int32_t value;
    int rc = ParseParams6(compiler, wrapper, &value);

    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }
        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    if (value == 0)
    {
        CStringAddRespNumber(g_message_cpu2arm, bpmParams.controllerVersion);
        return 0;
    }

    bpmParams.controllerVersion = value;
    bpmParams.tsv[0] = 'P';
    bpmParams.tsv[1] = 'M';
    bpmParams.tsv[2] = 'C';
    bpmParams.tsv[3] = '1';
    bpmParams.tsv[4] = '0';
    bpmParams.tsv[5] = '0';
    bpmParams.tsv[6] = '0';
    bpmParams.tsv[7] = 't';
    bpmParams.tsv[8] = 's';
    bpmParams.tsv[9] = 'v';

    bpmParams.tfv[0] = 'P';
    bpmParams.tfv[1] = 'M';
    bpmParams.tfv[2] = 'C';
    bpmParams.tfv[3] = '1';
    bpmParams.tfv[4] = '0';
    bpmParams.tfv[5] = '0';
    bpmParams.tfv[6] = '0';
    bpmParams.tfv[7] = 't';
    bpmParams.tfv[8] = 'f';
    bpmParams.tfv[9] = 'v';

    rc = PersistBPM();
    if (rc < 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PERSIST;
        return TEC_ERROR_PERSIST;
    }

    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }
    return 0;
}

int Command_BCP(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int rc = ParseParams5(compiler, wrapper);
    if (rc < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }
    RunstatusRegs.BCPFlag = 1;
    /* 初始化写程序的 Handle */
    // RunstatusRegs.codeFMHandle.readProgNameAddress = FLASH_PERSIST_PROGNUM_START;
    // RunstatusRegs.codeFMHandle.writeAddress = FLASH_PERSIST_PROGNUM_START;
    // RunstatusRegs.codeFMHandle.isOk = 0;
    /* 添加返回消息 */
    if (isMdi)
    {
        CStringAddRespOk(g_message_cpu2arm);
    }
    return 0;
}

int Command_IPA(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    int32_t value[4];

    int rc1 = ParseParams8(compiler, wrapper, value);

    if (rc1 < 0)
    {
        /* 参数解析错误 */
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /*  */
    if (value[0] == 0 && value[1] == 0 && value[2] == 0 && value[3] == 0)
    {
        /* 直接给ARM发送IPA相关信息 */
        /* g_message_cpu2arm 封装数组 传给上位机*/
        CStringAddRespUint16Array(g_message_cpu2arm, bpmParams.IPA, 4);
        return 0;
    }

    if (value[0] != 192 || value[1] != 168 || (value[2] < 0 || value[2] > 255) || (value[3] < 0 || value[3] > 255))
    {
        // 检查参数是否合法
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    bpmParams.IPA[0] = value[0];
    bpmParams.IPA[1] = value[1];
    bpmParams.IPA[2] = value[2];
    bpmParams.IPA[3] = value[3];

    rc1 = PersistBPM();
    if (rc1 < 0)
    {
        if (isMdi)
        {
            CStringAddRespFail(g_message_cpu2arm);
        }

        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PERSIST;
        return TEC_ERROR_PERSIST;
    }

    /* g_message_cpu2arm 封装数组 传给上位机*/
    CStringAddRespInt32Array(g_message_cpu2arm, value, 4);

    // CPU2ARMSendIPAMessage(bpmParams.IPA,4);

    return 0;
}

int Command_MCM(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    // if(isMdi){
    //     CStringAddRespFail(g_message_cpu2arm);
    //     g_params_tec = TCE_ERROR_NOT_UP_COMMAND;
    //     return TCE_ERROR_NOT_UP_COMMAND;
    // }
    // int j = 0;
    // /* 跳过空格 */
    // Strip(wrapper);
    // /* 寻找程序名长度 */
    // while(wrapper->buf[j] != 10 && wrapper->buf[j] != 59 && wrapper->buf[j] != 32){
    //     j++;
    // }
    // memcpy(compiler->token, wrapper->buf, j);
    // compiler->token[j] = 0;
    // compiler->tokenlen = j;

    // /* 从flash开头读取程序名 */
    // RunstatusRegs.codeFMHandle.readProgNameAddress = FLASH_PERSIST_PROGNAME_START;

    // int rc;
    // uint16_t len;
    // uint16_t buf[80];
    // uint32_t ProgAddr;
    // uint32_t nextProgName;
    // for(j = 0; j < subProgramNum; j++){
    //     //读取名字长度
    //     readUint16(&len,&RunstatusRegs.codeFMHandle.readProgNameAddress,1);
    //     //读取程序名字
    //     readLenData(buf,&RunstatusRegs.codeFMHandle.readProgNameAddress,80,len);
    //     //读取程序地址
    //     readUint32(&ProgAddr,&RunstatusRegs.codeFMHandle.readProgNameAddress,2);
    //     //校验程序名
    //     rc = checkName((char *)buf, len, compiler->token, compiler->tokenlen);
    //     if(rc == 0){
    //         if(!isFull(mystack)){
    //             /* 栈里存的是mcm下一条指令的地址，不是程序名下一条地址 */
    //             Push(mystack,(ElemType)RunstatusRegs.codeFMHandle.readProgContentsAddress);
    //             /* 程序名下一条指令地址 */
    //             RunstatusRegs.codeFMHandle.readProgContentsAddress = ProgAddr;
    //             break;
    //         }else{
    //             const char* NoneProgName = "stack over flow";
    //             CStringAddRespString(g_message_cpu2arm, NoneProgName, 15);
    //             return -1;
    //         }
    //     }
    //     //读取下一个程序名字地址
    //     readUint32(&nextProgName,&RunstatusRegs.codeFMHandle.readProgNameAddress,2);
    //     RunstatusRegs.codeFMHandle.readProgNameAddress = nextProgName;
    // }

    // if(rc < 0){
    //     //没有相应的程序名
    //     const char* NoneProgName = "Wrong Name,MCM Fail";
    //     CStringAddRespString(g_message_cpu2arm, NoneProgName, 19);
    //     return rc;
    // }

    // /*  清空wrapper */
    // wrapper->len = 0;

    return 0;
}

int Command_GOTO(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    // if(isMdi){
    //     CStringAddRespFail(g_message_cpu2arm);
    //     g_params_tec = TCE_ERROR_NOT_UP_COMMAND;
    //     return TCE_ERROR_NOT_UP_COMMAND;
    // }
    // int j = 0;
    // /* 跳过空格 */
    // Strip(wrapper);
    // /* 寻找程序名长度 */
    // while(wrapper->buf[j] != 10 && wrapper->buf[j] != 59 && wrapper->buf[j] != 32){
    //     j++;
    // }
    // memcpy(compiler->token, wrapper->buf, j);
    // compiler->token[j] = 0;
    // compiler->tokenlen = j;

    // /* 从flash开头读取程序名 */
    // RunstatusRegs.codeFMHandle.readProgNameAddress = FLASH_PERSIST_PROGNAME_START;

    // int rc;
    // uint16_t len;
    // uint16_t buf[80];
    // uint32_t ProgAddr;
    // uint32_t nextProgName;
    // for(j = 0; j < subProgramNum; j++){
    //     //读取名字长度
    //     readUint16(&len,&RunstatusRegs.codeFMHandle.readProgNameAddress,1);
    //     //读取程序名字
    //     readLenData(buf,&RunstatusRegs.codeFMHandle.readProgNameAddress,80,len);
    //     //读取程序地址
    //     readUint32(&ProgAddr,&RunstatusRegs.codeFMHandle.readProgNameAddress,2);
    //     //校验程序名
    //     rc = checkName((char *)buf, len, compiler->token, compiler->tokenlen);
    //     if(rc == 0){
    //         RunstatusRegs.codeFMHandle.readProgContentsAddress = ProgAddr;
    //         break;
    //     }
    //     //读取下一个程序名字地址
    //     readUint32(&nextProgName,&RunstatusRegs.codeFMHandle.readProgNameAddress,2);
    //     RunstatusRegs.codeFMHandle.readProgNameAddress = nextProgName;
    // }

    // if(rc < 0){
    //     //没有相应的程序名
    //     const char* NoneProgName = "Wrong Name,Goto Fail";
    //     CStringAddRespString(g_message_cpu2arm, NoneProgName, 20);
    //     return rc;
    // }

    // /*  清空wrapper */
    // wrapper->len = 0;

    return 0;
}

int Command_END(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    if (isMdi)
    {
        CStringAddRespFail(g_message_cpu2arm);
        g_params_tec = TCE_ERROR_NOT_UP_COMMAND;
        return TCE_ERROR_NOT_UP_COMMAND;
    }
    int rc = ParseParams5(compiler, wrapper);
    if (rc < 0)
    {
        /* 记录错误信息 */
        g_params_tec = TEC_ERROR_PROGRAM;
        return TEC_ERROR_PROGRAM;
    }

    /* 清除程序运行相关标志位 */
    g_params_is_execute_prog = 0;
    g_params_prog_counter = 0;

    g_params_wtm_count = 0;            // 暂停时间等待位
    g_params_commandline_ait_seq = -1; // IO等待位
    g_params_commandline_amp = 0;      // 点位运动等待位

    Init_Stack(mystack); // 重置栈
    return 0;
}

int Command_SEN(struct Compiler *compiler, struct StringWrapper *wrapper, bool isMdi)
{
    // if (isMdi) {
    //     CStringAddRespFail(g_message_cpu2arm);
    //     g_params_tec = TCE_ERROR_NOT_UP_COMMAND;
    //     return TCE_ERROR_NOT_UP_COMMAND;
    // }
    // int rc = ParseParams5(compiler, wrapper);
    // if (rc < 0) {
    //     /* 记录错误信息 */
    //     g_params_tec = TEC_ERROR_PROGRAM;
    //     return TEC_ERROR_PROGRAM;
    // }
    // if(!isEmpty(mystack)){
    //     Pop(mystack,RunstatusRegs.codeFMHandle.readProgContentsAddress);
    // }
    return 0;
}