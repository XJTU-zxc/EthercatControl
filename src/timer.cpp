
#include "variabledef.h"
#include "stdlib.h"
#include "common.h"
#include "public.h"
#include "compile.h"
#include "params.h"
#include "motioncontrolparams.h"
#include "paramscpp.h"
#include "timer.h"

uint16_t glExpZe = 0;

int et = 0;

uint16_t feedback1 = 0;
uint16_t feedback2 = 0;
uint16_t feedback3 = 0;
uint16_t feedback4 = 0;
uint16_t feedback5 = 0;
uint16_t feedback6 = 0;
uint16_t feedback7 = 0;
uint16_t feedback8 = 0;

uint16_t limit_io1 = 0;
uint16_t limit_io2 = 0;

// for test by zxc
int time_count = 0;
extern bool motionFlag; // 是否通过IgH 接口输出EtherCAT数据
// @author zxc
// @brief Set Single Axis Limit State
// @revision 1.0 by zxc on 2025-07-11
void setLimitBit(int index, bool value)
{
    switch (index)
    {
    case LIMIT_XP:
        RunstatusRegs.Alarm.softlimit.bit.xp = value ? 1 : 0;
        break;
    case LIMIT_XR:
        RunstatusRegs.Alarm.softlimit.bit.xr = value ? 1 : 0;
        break;
    case LIMIT_YP:
        RunstatusRegs.Alarm.softlimit.bit.yp = value ? 1 : 0;
        break;
    case LIMIT_YR:
        RunstatusRegs.Alarm.softlimit.bit.yr = value ? 1 : 0;
        break;
    case LIMIT_ZP:
        RunstatusRegs.Alarm.softlimit.bit.zp = value ? 1 : 0;
        break;
    case LIMIT_ZR:
        RunstatusRegs.Alarm.softlimit.bit.zr = value ? 1 : 0;
        break;
    case LIMIT_AP:
        RunstatusRegs.Alarm.softlimit.bit.ap = value ? 1 : 0;
        break;
    case LIMIT_AR:
        RunstatusRegs.Alarm.softlimit.bit.ar = value ? 1 : 0;
        break;
    case LIMIT_BP:
        RunstatusRegs.Alarm.softlimit.bit.bp = value ? 1 : 0;
        break;
    case LIMIT_BR:
        RunstatusRegs.Alarm.softlimit.bit.br = value ? 1 : 0;
        break;
    case LIMIT_CP:
        RunstatusRegs.Alarm.softlimit.bit.cp = value ? 1 : 0;
        break;
    case LIMIT_CR:
        RunstatusRegs.Alarm.softlimit.bit.cr = value ? 1 : 0;
        break;
    case LIMIT_WP:
        RunstatusRegs.Alarm.softlimit.bit.wp = value ? 1 : 0;
        break;
    case LIMIT_WR:
        RunstatusRegs.Alarm.softlimit.bit.wr = value ? 1 : 0;
        break;
    case LIMIT_UP:
        RunstatusRegs.Alarm.softlimit.bit.up = value ? 1 : 0;
        break;
    case LIMIT_UR:
        RunstatusRegs.Alarm.softlimit.bit.ur = value ? 1 : 0;
        break;
    }
}
// @author zxc
// @brief Set All Axis Limit State
// @revision 1.0 by zxc on 2025-07-11
void SetLimitState(int num = SLAVE_NUM)
{
    int limit_num = 0;
    for (size_t i = 0; i < num; i++)
    {
        int p = 2 * i;
        int n = 2 * i + 1;
        // 超过正向行程极限
        if ((*g_params_axisRealPositionPtr[i] > ParamterUnRegs.ParamterRegs.AxisparmRegs.PSoftLimit[i]) || ((g_io_plimitIO) & (1 << (8 + i))) != 0)
        {
            setLimitBit(p, 1);
            nowIfLimit[p] = 1;
            limit_num++;
        }
        else
        {
            setLimitBit(p, 0);
            nowIfLimit[p] = 0;
        }
        // 超过负向行程极限
        if ((*g_params_axisRealPositionPtr[i] < ParamterUnRegs.ParamterRegs.AxisparmRegs.NSoftLimit[i]) || ((g_io_nlimitIO) & (1 << i)) != 0)
        {
            setLimitBit(n, 1);
            nowIfLimit[n] = 1;
            limit_num++;
        }
        else
        {
            setLimitBit(n, 1);
            nowIfLimit[n] = 0;
        }
    }
    // 有硬限位或者软限位，暂停程序，剩下程序不执行
    if (limit_num)
    {
        g_params_is_execute_prog = 0; // 计数器清 0
        g_params_prog_counter = 0;
        g_params_wtm_count = 0; // 等待时间置位
    }
}
// @author zxc
// @brief For Debug, Print pulse data
// @revision 1.0 by zxc on 2025-07-11
void print_pos_x_to_u(struct inter_pos *InterPos)
{
    const size_t offsets[] = {
        offsetof(struct inter_pos, pos_x),
        offsetof(struct inter_pos, pos_y),
        offsetof(struct inter_pos, pos_z),
        offsetof(struct inter_pos, pos_a),
        offsetof(struct inter_pos, pos_b),
        offsetof(struct inter_pos, pos_c),
        offsetof(struct inter_pos, pos_w),
        offsetof(struct inter_pos, pos_u)};

    const char *names[] = {
        "pos_x", "pos_y", "pos_z", "pos_a",
        "pos_b", "pos_c", "pos_w", "pos_u"};
    // 打印第一行：x, y, z, a
    for (int i = 0; i < 4; ++i)
    {
        int32_t *value = (int32_t *)((char *)InterPos + offsets[i]);
        printf("[INFO] %s: %10d  ", names[i], *value);
    }
    printf("\n");
    // 打印第二行：b, c, w, u
    for (int i = 4; i < 8; ++i)
    {
        int32_t *value = (int32_t *)((char *)InterPos + offsets[i]);
        printf("[INFO] %s: %10d  ", names[i], *value);
    }
    printf("\n\n");
}

// 判断轴的位置是否发生了变化  zjq
bool isChange(void)
{
    if (
        PostionRegs.real_posbk.pos_x != 0 ||
        PostionRegs.real_posbk.pos_y != 0 ||
        PostionRegs.real_posbk.pos_z != 0 ||
        PostionRegs.real_posbk.pos_a != 0 ||
        PostionRegs.real_posbk.pos_b != 0 ||
        PostionRegs.real_posbk.pos_c != 0 ||
        PostionRegs.real_posbk.pos_w != 0 ||
        PostionRegs.real_posbk.pos_u != 0)
    {
        return true;
    }
    return false;
}

void DealWith_4096us(void)
{
    ExecuteBGMCommand();
    /* 判断是否需要急停 */
    if (((g_io_nlimitIO) & (1 << 8)) != 0)
    {
        ExcuteEstCommand();
    }

    time_count++;
    static uint16_t realMode = 3;
    int32_t temp_pos1, temp_pos2, temp_pos3;
    int32_t temp_pos4, temp_pos5, temp_pos6, temp_pos7, temp_pos8;

    uint16_t TBAxis, temp1, cmdMode;
    uint16_t K2, K1;
    // 更新上上次位置 zjq
    if (isChange())
    {
        PostionRegs.last_real_posbk = PostionRegs.real_posbk;
    }

    // 更新上一次的位置
    PostionRegs.real_posbk = PostionRegs.real_pos;

    PostionRegs.LinPos = PostionRegs.real_pos;

    cmdMode = RunstatusRegs.RunStatusFlag.bit.Type;
    switch (realMode)
    {
    case 0:
        realMode = cmdMode;
        break;
    case 2: // 点动模式
        if (!RunstatusRegs.Alarm.limit.all)
            DD();
        if (cmdMode != 2)
        {
            realMode = cmdMode;
        }
        break;
    case 3: // JOG控制
        if (cmdMode != 3)
        {
            JOGRegs.JogSel = 0;
        }
        if (JOG(RunstatusRegs.Alarm.limit.all) == 0) // means not busy..
        {
            if (cmdMode != 3)
            {
                realMode = cmdMode;
            }
        }
        break;
    case 4: // 自动运行控制
    case 5: // 自动运行控制
    case 8: // 自动运行控制
        AutoRun();
        if ((cmdMode != 4) && (cmdMode != 5) && (cmdMode != 8))
        {
            realMode = cmdMode;
        }

        break;
    case 6: // 回参控制
        if (!RunstatusRegs.Alarm.limit.all)
            RetRefPoint();

        if (cmdMode != 6)
        {
            realMode = cmdMode;
        }

        break;
    case 7: // 电子手轮
        if (!RunstatusRegs.Alarm.limit.all)
            // ehand();
            if (cmdMode != 7)
            {
                realMode = cmdMode;
            }
        break;
    default:
        realMode = 0;
    }
    //	MCode();

    /* JOG模式 */
    if (g_params_jog_enable == 1)
    {
        MyJOG2();
    }

    // CorpConv();

    // 计算零件坐标
    for (size_t i = 0; i < SLAVE_NUM; i++)
    {
        *g_params_wpPositionPtr[i] = *g_params_axisRealPositionPtr[i] - *g_params_axisPositionPtr[i];
    }
    // PostionRegs.LinPos.pos_x = PostionRegs.real_pos.pos_x - EncodeRegs.AsReg.X;
    // PostionRegs.LinPos.pos_y = PostionRegs.real_pos.pos_y - EncodeRegs.AsReg.Y;
    // PostionRegs.LinPos.pos_z = PostionRegs.real_pos.pos_z - EncodeRegs.AsReg.Z;
    // PostionRegs.LinPos.pos_a = PostionRegs.real_pos.pos_a - EncodeRegs.AsReg.A;
    // PostionRegs.LinPos.pos_b = PostionRegs.real_pos.pos_b - EncodeRegs.AsReg.B;
    // PostionRegs.LinPos.pos_c = PostionRegs.real_pos.pos_c - EncodeRegs.AsReg.C;
    // PostionRegs.LinPos.pos_w = PostionRegs.real_pos.pos_w - EncodeRegs.AsReg.W;
    // PostionRegs.LinPos.pos_u = PostionRegs.real_pos.pos_u - EncodeRegs.AsReg.U;

    if (g_params_motion_mode == 1)
    {
        /* 后续放入背景程序中发送消息 */
        if (!g_interpDataQueue.IsFull() && interpNum != 0)
        {
            InterpData *idata = g_interpDataQueue.Pop();
            if (idata != NULL)
            {
                g_interpMotionBuffer.Push(idata);
            }
            /* 执行完成后会自动出队 */
            g_interpMotionBuffer.Execute();
            if ((g_interpMotionBuffer.IsEmpty() && interpNum == 0 && g_interpDataQueue.IsEmpty()))
            {
                /* 队列为空，重置状态wyf */
                init_tan_K_flag = false;
                /* 队列为空，没有进行插补运动 */
                g_params_interp_is_running = 0;

                /* 清空插补轴 */
                g_params_interp_axis_enable = 0;
                /* 结束正切模式wyf */
                g_params_vtn_enable = 0;
                /* 结束插补模式 */
                g_params_motion_mode = 0;
                /* 清空插补队列和缓冲队列中的数据 */
                g_interpDataQueue.Clear();
                g_interpMotionBuffer.Clear();
                /* 给CPU2发送信号，停止发送插补数据过来 */
                // interpNum = 0;
                // CPU1SendMessageToCPU2(3, 1);
                /* 清空平面轴信息 */
                g_params_plate_interp_axis[0] = -1;
                g_params_plate_interp_axis[1] = -1;
                g_params_plate_interp_axis[2] = -1;
            }
            else
            {
                /* 如果队列中有数据，说明正在进行插补运动 */
                g_params_interp_is_running = 1;
            }
        }
    }

    // 计算输出脉冲
    motionFlag = false;
    for (size_t i = 0; i < SLAVE_NUM; i++)
    {
        *g_params_axisPulseData[i] = *g_params_axisRealPositionPtr[i] - *g_params_axisLastPulse[i];
        // 用于调试以及后续判断是否输出数据到电机
        if (*g_params_axisPulseData[i] != 0)
        {
            motionFlag = true; // 只要有一个脉冲数据大于0，就设置标志
        }
    }
    // if (motionFlag)
    // {
    //     printf("[INFO] 输出脉冲: \n");
    //     print_pos_x_to_u(&PostionRegs.out_imp);
    // }
    // PostionRegs.out_imp.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.real_posbk.pos_x;
    // PostionRegs.out_imp.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.real_posbk.pos_y;
    // PostionRegs.out_imp.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.real_posbk.pos_z;
    // PostionRegs.out_imp.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.real_posbk.pos_a;
    // PostionRegs.out_imp.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.real_posbk.pos_b;
    // PostionRegs.out_imp.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.real_posbk.pos_c;
    // PostionRegs.out_imp.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.real_posbk.pos_w;
    // PostionRegs.out_imp.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.real_posbk.pos_u;

    // if (time_count % 100 == 0)
    // {
    //     // 每0.4*250=100ms打印一次位置
    //     printf("[INFO] 零件坐标: \n");
    //     print_pos_x_to_u(&PostionRegs.LinPos);
    //     time_count = 0;
    // }

    // 间隙补偿
    if (g_bkc_flag == 1)
    {
        RC();
    }

    if (g_dec_flag == 1)
    {
        myPC();
    }

    // 轴限位处理
    SetLimitState();
}
void delay(int n)
{
    while (--n)
        ;
}

uint16_t readMemory2(uint16_t *address)
{
    uint16_t res = 0;
    res = *address;
    return res;
}

int flag = 10;
void Timer10_ISR() // 50us int
{
    static int ioCount = 0;
    static int encodeCount = 0;
    /* 输出脉冲 */

    /* 读取正负限位 io */
    // g_io_plimitIO = readMemory2((uint16_t*)0x00002010);  //低八位为回零开关
    // g_io_nlimitIO = readMemory2((uint16_t*)0x00002018);
    // z_pulse_param = readMemory2((uint16_t*)0x00002020);  //低八位初始为1

    // for (int idx = 0; idx < 8; idx++) {
    //     if( ( (fid_enable_flag & (1 << idx)) != 0 ) && ( ((z_pulse_param & (1 << idx)) != 0) ) && ( (g_io_plimitIO & (1 << idx)) == 0 ) ) {
    //         z_pulse_param_buf |= (1 << idx);
    //     }
    // }

    if (g_params_wtm_count > 0)
    {
        /* 暂停时间 -1 */
        --g_params_wtm_count;
    }

    /* 读取编码器数据 */
    ++encodeCount;
    if (encodeCount >= 10)
    {
        // 500us
        RobotCoreReadEncode(&robotCore);
        g_speedFilterQueueA.Push(GetEncodeSpeed(robotCore.encode, AXIS_X));
        g_speedFilterQueueB.Push(GetEncodeSpeed(robotCore.encode, AXIS_Y));
        g_speedFilterQueueC.Push(GetEncodeSpeed(robotCore.encode, AXIS_Z));
        g_speedFilterQueueD.Push(GetEncodeSpeed(robotCore.encode, AXIS_A));
        g_speedFilterQueueE.Push(GetEncodeSpeed(robotCore.encode, AXIS_B));
        g_speedFilterQueueF.Push(GetEncodeSpeed(robotCore.encode, AXIS_C));
        g_speedFilterQueueG.Push(GetEncodeSpeed(robotCore.encode, AXIS_U));
        g_speedFilterQueueH.Push(GetEncodeSpeed(robotCore.encode, AXIS_W));

        encodeCount = 0;
    }

    /* 判断是否需要急停 */
    if (((g_io_nlimitIO) & (1 << 8)) != 0)
    {
        ExcuteEstCommand();
    }

    // ++ioCount;
    // if (ioCount >= 10) {
    //     IPC_Loop();     // 500us
    //     ioCount = 0;
    // }

    DealWith_4096us(); // 0.4ms一次插补计算
}
uint16_t realLoops;                       // test22,
uint16_t Grun = 0, Gstop = 0, refRun = 0; // GcodeResetEnalbe,
uint16_t glZeroPosi, glZE, glAxisAlarm, glAxisLimit, glABE;
uint16_t StatusIO[10], StatusIOXor[10], StatusIOOld[10];