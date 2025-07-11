
#include "variabledef.h"
// #include "comp/accSFloat/accSFloat.h"
#include "accSint.h"
#include "accLSint.h"
#include "accMulAxis.h"
#include "circle.h"
#include "jog.h"
#include "returnRef.h"

#include "stdlib.h"
#include "params.h"
#include "InterpMotionBlock.h"

#define TEST_SINGLE_AXIS_MOVE

extern uint16_t glAxisLimit;

void RpdRun(void);
long roundFtoL(float in);
uint16_t RetRefPoint(void);
//********************************************************
long roundFtoL(float in)
{
    if (in < 0.0)
        return (long)(in - 0.5);
    else
        return (long)(in + 0.5);
}
//*******************************************************
/* 8轴点位运动  2023.07.24 */
uint16_t MyRpdRun2(uint16_t cmdRun)
{
    static CaccSint accX, accY, accZ, accA, accB, accC, accW, accU;
    static int32_t bakX, bakY, bakZ, bakA, bakB, bakC, bakW, bakU;
    /* 点位运动计算类数组 */
    static CaccSint accAxises[8] = {accX, accY, accZ, accA, accB, accC, accW, accU};
    /* 保存上一次的运动状态 */
    static int32_t bakAxises[8] = {bakX, bakY, bakZ, bakA, bakB, bakC, bakW, bakU};
    static uint16_t phases[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    uint16_t busy = 1;

    int i;
    for (i = 0; i < 8; i++)
    {
        switch (phases[i])
        {
        case 0:
            /* 重置状态，重新设置参数 */
            accAxises[i].reset();
            accAxises[i].in.length = *g_params_axisPositionPtr[i] - *g_params_axisRealPositionPtr[i];
            accAxises[i].in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[i];
            accAxises[i].in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[i];
            accAxises[i].in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[i] * g_params_mit[i];
            accAxises[i].sureInput();
            /* 保存现在的状态 */
            bakAxises[i] = *g_params_axisRealPositionPtr[i];
            phases[i] = 1;
        case 1:
#ifdef TEST_SINGLE_AXIS_MOVE
            if (i == 0)
            {
                accAxises[i].run();
            }
#else
            accAxises[i].run(i);
#endif
            /* 如果要输出正向脉冲并且当前没有正限位且没有STM停轴*/ /* 如果要输出负向脉冲并且当前没有负限位且没有STM停轴 */
            if (STMAxises[i] != 1 && ((accAxises[i].view.position > 0 && nowIfLimit[i * 2] != 1) || (accAxises[i].view.position < 0 && nowIfLimit[i * 2 + 1] != 1)))
            {
                *g_params_axisRealPositionPtr[i] = bakAxises[i] + accAxises[i].view.position;
            }
            else if (STMAxises[i] == 1 || (accAxises[i].view.position > 0 && nowIfLimit[i * 2] == 1) || (accAxises[i].view.position < 0 && nowIfLimit[i * 2 + 1] == 1))
            {
                // 如果有STM或者正转正限位 或者反转反限位 需要制动
                accAxises[i].in.pauseCmd = 2;
                phases[i] = 2;
            }
            break;
        case 2:
            /* 如果已经停下来了，直接退出 */
            if (accAxises[i].status == END)
            {
                break;
            }
            /* 减速 */
            accAxises[i].run();
            *g_params_axisRealPositionPtr[i] = bakAxises[i] + accAxises[i].view.position;

            break;
        default:
            phases[i] = 0;
        }

        if (accAxises[i].status == END)
        {
            /* 检查当前轴是否运动 */
            g_axis_is_running &= (~(1 << i));
        }
    }
    /* 如果轴全部停止运动，该条指令结束 */
    if (g_axis_is_running == 0)
    {
        /* 重置流程 */
        for (i = 0; i < 8; i++)
        {
            phases[i] = 0;
        }
        busy = 0; // means not busy, can do other things.
        RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
    }
    else
    {
        busy = 1;
    }
    return busy;
}

/* 旧版 弃用 */
uint16_t RpdRun(uint16_t cmdRun)
{ // static 	CaccSFloat //,accC;
    // static  CaccLSint accX;
    static CaccSint accX, accY, accZ, accA, accB, accC, accW, accU; //
    static int32_t bakX, bakY, bakZ, bakA, bakB, bakC, bakW, bakU;
    static uint16_t phase = 0;
    uint16_t busy = 1;

    /* 如果有急停，停止指令位置更新 */
    if (((g_io_nlimitIO) & (1 << 8)) != 0)
    {
        accX.status = END;
        accY.status = END;
        accZ.status = END;
        accA.status = END;
        accB.status = END;
        accC.status = END;
        accW.status = END;
        accU.status = END;
        g_params_est = 0;
    }

    switch (phase)
    {
    case 0:
        if (cmdRun == 0)
        {
            break;
        }
        else
        {
            accX.reset();
            accX.in.length = EncodeRegs.AsReg.X - PostionRegs.real_pos.pos_x;
            accX.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[0];       ///(100.0*60.0);
            accX.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[0];             /// 100.0;//0.08;
            accX.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[0] * g_params_mit[0]; /// 100.0;//0.08;
            accX.sureInput();
            bakX = PostionRegs.real_pos.pos_x;
            //----------------------------------
            accY.reset();
            accY.in.length = EncodeRegs.AsReg.Y - PostionRegs.real_pos.pos_y;
            accY.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[1];       ///(100.0*60.0);
            accY.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[1];             /// 100.0;//0.08;
            accY.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[1] * g_params_mit[1]; /// 100.0;//0.08;
            accY.sureInput();
            bakY = PostionRegs.real_pos.pos_y;
            //----------------------------------
            accZ.reset();
            accZ.in.length = EncodeRegs.AsReg.Z - PostionRegs.real_pos.pos_z;
            accZ.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[2];       ///(100.0*60.0);
            accZ.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[2];             /// 100.0;//0.08;
            accZ.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[2] * g_params_mit[2]; /// 100.0;//0.08;
            accZ.sureInput();
            bakZ = PostionRegs.real_pos.pos_z;
            //----------------------------------
            accA.reset();
            accA.in.length = EncodeRegs.AsReg.A - PostionRegs.real_pos.pos_a;
            accA.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[3];       ///(100.0*60.0);
            accA.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[3];             /// 100.0;//0.08;
            accA.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[3] * g_params_mit[3]; /// 100.0;//0.08;
            accA.sureInput();
            bakA = PostionRegs.real_pos.pos_a;
            //----------------------------------
            accB.reset();
            accB.in.length = EncodeRegs.AsReg.B - PostionRegs.real_pos.pos_b;
            accB.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[4];       ///(100.0*60.0);
            accB.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[4];             /// 100.0;//0.08;
            accB.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[4] * g_params_mit[4]; /// 100.0;//0.08;
            accB.sureInput();
            bakB = PostionRegs.real_pos.pos_b;
            //----------------------------------
            accC.reset();
            accC.in.length = EncodeRegs.AsReg.C - PostionRegs.real_pos.pos_c;
            accC.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[5];       ///(100.0*60.0);
            accC.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[5];             /// 100.0;//0.08;
            accC.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[5] * g_params_mit[5]; /// 100.0;//0.08;
            accC.sureInput();
            bakC = PostionRegs.real_pos.pos_c;
            //----------------------------------
            accW.reset();
            accW.in.length = EncodeRegs.AsReg.W - PostionRegs.real_pos.pos_w;
            accW.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[6];       ///(100.0*60.0);
            accW.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[6];             /// 100.0;//0.08;
            accW.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[6] * g_params_mit[6]; /// 100.0;//0.08;
            accW.sureInput();
            bakW = PostionRegs.real_pos.pos_w;
            //----------------------------------
            accU.reset();
            accU.in.length = EncodeRegs.AsReg.U - PostionRegs.real_pos.pos_u;
            accU.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[7];       ///(100.0*60.0);
            accU.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[7];             /// 100.0;//0.08;
            accU.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[7] * g_params_mit[7]; /// 100.0;//0.08;
            accU.sureInput();
            bakU = PostionRegs.real_pos.pos_u;
            //----------------------------------
            phase = 1;
        }
    // no break;
    case 1:
        accX.run();
        PostionRegs.real_pos.pos_x = bakX + accX.view.position;
        //---------------------------------------------------------------------------
        accY.run();
        PostionRegs.real_pos.pos_y = bakY + accY.view.position;
        //---------------------------------------------------------------------------
        accZ.run();
        PostionRegs.real_pos.pos_z = bakZ + accZ.view.position;
        //---------------------------------------------------------------------------
        accA.run();
        PostionRegs.real_pos.pos_a = bakA + accA.view.position;
        //---------------------------------------------------------------------------
        accB.run();
        PostionRegs.real_pos.pos_b = bakB + accB.view.position;
        //---------------------------------------------------------------------------
        accC.run();
        PostionRegs.real_pos.pos_c = bakC + accC.view.position;
        //-----------------------------------------------------------------------------
        accW.run();
        PostionRegs.real_pos.pos_w = bakW + accW.view.position;
        //-----------------------------------------------------------------------------
        accU.run();
        PostionRegs.real_pos.pos_u = bakU + accU.view.position;
        //-----------------------------------------------------------------------------
        if (accX.status == END)
        {
            /* 第一位置为 0 */
            g_axis_is_running &= 0xfffe;
        }
        if (accY.status == END)
        {
            /* 第二位置为 0 */
            g_axis_is_running &= 0xfffd;
        }
        if (accZ.status == END)
        {
            /* 第三位置为 0 */
            g_axis_is_running &= 0xfffb;
        }
        if (accA.status == END)
        {
            /* 第四位置为 0 */
            g_axis_is_running &= 0xfff7;
        }
        if (accB.status == END)
        {
            /* 第五位置为 0 */
            g_axis_is_running &= 0xffef;
        }
        if (accC.status == END)
        {
            /* 第六位置为 0 */
            g_axis_is_running &= 0xffdf;
        }
        if (accW.status == END)
        {
            /* 第七位置为 0 */
            g_axis_is_running &= 0xffbf;
        }
        if (accU.status == END)
        {
            /* 第八位置为 0 */
            g_axis_is_running &= 0xff7f;
        }

        /* 如果轴全部停止运动，该条指令结束 */
        if (g_axis_is_running == 0)
        {
            phase = 0; // can deal another line G code.
            busy = 0;  // means not busy, can do other things.
            RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
        }
        else
        {
            if (cmdRun == 0) // need brake
            {
                if ((~glAxisLimit) & 0x03ff) // need stop
                                             // phase=3;
                {
                    accX.in.pauseCmd = 2; // brake Hi speed  5 times normal acc
                    accY.in.pauseCmd = 2;
                    accZ.in.pauseCmd = 2;
                    accA.in.pauseCmd = 2;
                    accB.in.pauseCmd = 2;
                    accC.in.pauseCmd = 2;
                    accW.in.pauseCmd = 2;
                    accU.in.pauseCmd = 2;
                }
                else
                {
                    accX.in.pauseCmd = 1;
                    accY.in.pauseCmd = 1;
                    accZ.in.pauseCmd = 1;
                    accA.in.pauseCmd = 1;
                    accB.in.pauseCmd = 1;
                    accC.in.pauseCmd = 1;
                    accW.in.pauseCmd = 1;
                    accU.in.pauseCmd = 1;
                }
                phase = 2;
            }
            // follow is the soft limit judge
            if (RunstatusRegs.Alarm.softlimit.all)
            // need enter a stop status
            {
                RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
                RunstatusRegs.isCmdCompelete = 2;
                phase = 4; // enter a stop status, and wait for cmd stop
            }
            busy = 1; // means busy.
        }
        break;
    case 2: // brake process
        accX.run();
        PostionRegs.real_pos.pos_x = bakX + accX.view.position;
        //---------------------------------------------------------------------------
        accY.run();
        PostionRegs.real_pos.pos_y = bakY + accY.view.position;
        //---------------------------------------------------------------------------
        accZ.run();
        PostionRegs.real_pos.pos_z = bakZ + accZ.view.position;
        //---------------------------------------------------------------------------
        accA.run();
        PostionRegs.real_pos.pos_a = bakA + accA.view.position;
        //---------------------------------------------------------------------------
        accB.run();
        PostionRegs.real_pos.pos_b = bakB + accB.view.position;
        //---------------------------------------------------------------------------
        accC.run();
        PostionRegs.real_pos.pos_c = bakC + accC.view.position;
        //---------------------------------------------------------------------------
        accW.run();
        PostionRegs.real_pos.pos_w = bakW + accW.view.position;
        //---------------------------------------------------------------------------
        accU.run();
        PostionRegs.real_pos.pos_u = bakU + accU.view.position;
        //---------------------------------------------------------------------------
        if ((accX.status == END) && (accY.status == END) && (accZ.status == END) && (accA.status == END) && (accB.status == END) && (accC.status == END) && (accW.status == END) && (accU.status == END))
        {
            phase = 0; // brake finished.
            busy = 0;  // means not busy, can do other things.
            RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
        }
        else
        {
            busy = 1;
            if ((~glAxisLimit) & 0x03ff) // need stop
            {                            // phase=3;
                accX.in.pauseCmd = 2;
                accY.in.pauseCmd = 2;
                accZ.in.pauseCmd = 2;
                accA.in.pauseCmd = 2;
                accB.in.pauseCmd = 2;
                accC.in.pauseCmd = 2;
                accW.in.pauseCmd = 2;
                accU.in.pauseCmd = 2;
            }
            // follow is the soft limit judge
            //	else
            if (RunstatusRegs.Alarm.softlimit.all)
            // need enter a stop status
            {
                phase = 4; // enter a stop status, and wait for cmd stop
            }
        }
        break;
    case 3:
        busy = 0;
        phase = 0;
        break;
    case 4:
        if (cmdRun == 0) // need stop
        {
            busy = 0;
            phase = 0;
        }
        else if (RunstatusRegs.Alarm.softlimit.all == 0)
        {
            phase = 0;
        }
        else
        {
            busy = 1;
        }

        break;
    default:
        phase = 0;
    }
    return busy;
}

//***********************************************************
uint16_t LineChabu(uint16_t cmdRun)
{
    static CaccMulAxis obj;
    static uint16_t phase = 0;
    uint16_t busy = 1;

    // if(RunstatusRegs.run_cnt==1)
    switch (phase)
    {
    case 0:
        RunstatusRegs.st_end.x_st = PostionRegs.real_pos.pos_x;
        RunstatusRegs.st_end.y_st = PostionRegs.real_pos.pos_y;
        RunstatusRegs.st_end.z_st = PostionRegs.real_pos.pos_z;
        RunstatusRegs.st_end.a_st = PostionRegs.real_pos.pos_a;
        RunstatusRegs.st_end.b_st = PostionRegs.real_pos.pos_b;
        RunstatusRegs.st_end.c_st = PostionRegs.real_pos.pos_c;
        RunstatusRegs.st_end.w_st = PostionRegs.real_pos.pos_w;
        RunstatusRegs.st_end.u_st = PostionRegs.real_pos.pos_u;
        RunstatusRegs.st_end.x_end = EncodeRegs.AsReg.X;
        RunstatusRegs.st_end.y_end = EncodeRegs.AsReg.Y;
        RunstatusRegs.st_end.z_end = EncodeRegs.AsReg.Z;
        RunstatusRegs.st_end.a_end = EncodeRegs.AsReg.A;
        RunstatusRegs.st_end.b_end = EncodeRegs.AsReg.B;
        RunstatusRegs.st_end.c_end = EncodeRegs.AsReg.C;
        RunstatusRegs.st_end.w_end = EncodeRegs.AsReg.W;
        RunstatusRegs.st_end.u_end = EncodeRegs.AsReg.U;
        obj.reset();
        obj.in.lengthX = EncodeRegs.AsReg.X - PostionRegs.real_pos.pos_x;
        obj.in.lengthY = EncodeRegs.AsReg.Y - PostionRegs.real_pos.pos_y;
        obj.in.lengthZ = EncodeRegs.AsReg.Z - PostionRegs.real_pos.pos_z;
        obj.in.lengthA = EncodeRegs.AsReg.A - PostionRegs.real_pos.pos_a;
        obj.in.lengthB = EncodeRegs.AsReg.B - PostionRegs.real_pos.pos_b;
        obj.in.lengthC = EncodeRegs.AsReg.C - PostionRegs.real_pos.pos_c;
        obj.in.lengthW = EncodeRegs.AsReg.W - PostionRegs.real_pos.pos_w;
        obj.in.lengthU = EncodeRegs.AsReg.U - PostionRegs.real_pos.pos_u;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed = (float)ThreeWordsReg.ChabuMoveSpeed / 9155 / 10000 * 60;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc = (float)ThreeWordsReg.VACSpeed / 9155 / 10000 * 60;
        obj.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed * g_params_vsr; ///(100.0*60.0);
        obj.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc;                      /// 100.0;//0.08;
        // obj.in.setJ=(float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[0];///100.0;//0.08;
        obj.in.setJ = 0.08;
        obj.sureInput();
        phase = 1;
    //  RunstatusRegs.run_cnt=0;
    // no break;
    case 1:
        obj.run();
        PostionRegs.real_pos.pos_x = RunstatusRegs.st_end.x_st + obj.view.pos_x;
        PostionRegs.real_pos.pos_y = RunstatusRegs.st_end.y_st + obj.view.pos_y;
        PostionRegs.real_pos.pos_z = RunstatusRegs.st_end.z_st + obj.view.pos_z;
        PostionRegs.real_pos.pos_a = RunstatusRegs.st_end.a_st + obj.view.pos_a;
        PostionRegs.real_pos.pos_b = RunstatusRegs.st_end.b_st + obj.view.pos_b;
        PostionRegs.real_pos.pos_c = RunstatusRegs.st_end.c_st + obj.view.pos_c;
        PostionRegs.real_pos.pos_w = RunstatusRegs.st_end.w_st + obj.view.pos_w;
        PostionRegs.real_pos.pos_u = RunstatusRegs.st_end.u_st + obj.view.pos_u;

        if ((RunstatusRegs.st_end.x_end == PostionRegs.real_pos.pos_x) && (RunstatusRegs.st_end.y_end == PostionRegs.real_pos.pos_y) &&
            (RunstatusRegs.st_end.z_end == PostionRegs.real_pos.pos_z) && (RunstatusRegs.st_end.a_end == PostionRegs.real_pos.pos_a) &&
            (RunstatusRegs.st_end.b_end == PostionRegs.real_pos.pos_b) && (RunstatusRegs.st_end.c_end == PostionRegs.real_pos.pos_c) &&
            (RunstatusRegs.st_end.w_end == PostionRegs.real_pos.pos_w) && (RunstatusRegs.st_end.u_end == PostionRegs.real_pos.pos_u))
        {
            busy = 0;
            RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
            RunstatusRegs.isCmdCompelete = 2;
            phase = 0;
        }
        else
        {
            if (cmdRun == 0) // need brake
            {
                if ((~glAxisLimit) & 0x03ff) // need stop
                    phase = 3;
                else
                {
                    obj.in.pauseCmd = 1;
                    phase = 2;
                }
            }
            if (RunstatusRegs.Alarm.softlimit.all)
            // need enter a stop status
            {
                RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
                RunstatusRegs.isCmdCompelete = 2;
                phase = 4; // enter a stop status, and wait for cmd stop
            }
            busy = 1; // means busy.
        }
        break;
    case 2: // brake process
        obj.run();
        PostionRegs.real_pos.pos_x = RunstatusRegs.st_end.x_st + obj.view.pos_x;
        PostionRegs.real_pos.pos_y = RunstatusRegs.st_end.y_st + obj.view.pos_y;
        PostionRegs.real_pos.pos_z = RunstatusRegs.st_end.z_st + obj.view.pos_z;
        PostionRegs.real_pos.pos_a = RunstatusRegs.st_end.a_st + obj.view.pos_a;
        PostionRegs.real_pos.pos_b = RunstatusRegs.st_end.b_st + obj.view.pos_b;
        PostionRegs.real_pos.pos_c = RunstatusRegs.st_end.c_st + obj.view.pos_c;
        PostionRegs.real_pos.pos_w = RunstatusRegs.st_end.w_st + obj.view.pos_w;
        PostionRegs.real_pos.pos_u = RunstatusRegs.st_end.u_st + obj.view.pos_u;
        //---------------------------------------------------------------------------
        if (obj.status == END)
        {
            phase = 0; // brake finished.
            busy = 0;  // means not busy, can do other things.
        }
        else
        {
            busy = 1;
            if ((~glAxisLimit) & 0x03ff) // need stop
                phase = 3;
            // follow is the soft limit judge
            else if (RunstatusRegs.Alarm.softlimit.all)
            // need enter a stop status
            {
                phase = 4; // enter a stop status, and wait for cmd stop
            }
        }
        break;
    case 3:
        busy = 0;
        phase = 0;
        break;
    case 4:
        if (cmdRun == 0) // need stop
        {
            busy = 0;
            phase = 0;
        }
        else
            busy = 1;
        break;
    default:
        phase = 0;
    }
    //----------------------------------------
    return busy;
}

//******************************************************

uint16_t ShunNi_Cir(uint16_t cmdRun)
{
    static Ccircle obj;
    static uint16_t phase = 0;
    uint16_t busy = 1;

    if (obj.status == END)
    { // if (cmdAutoReset==1)
      //   {   cmdAutoReset=0;
      //       phase=0;
      //   }
    }
    // if(RunstatusRegs.run_cnt==1)
    switch (phase)
    {
    case 0:
        if (EncodeRegs.AsReg.Plate == 17)
        {
            CirCleInterRegs.st_end_cir.x_st = PostionRegs.real_pos.pos_x; // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st = PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.x_end = EncodeRegs.AsReg.X;
            CirCleInterRegs.st_end_cir.y_end = EncodeRegs.AsReg.Y;
            CirCleInterRegs.st_end_cir.cen_x = EncodeRegs.AsReg.I + PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.cen_y = EncodeRegs.AsReg.J + PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.velo = EncodeRegs.AsReg.F;
        }
        if (EncodeRegs.AsReg.Plate == 18)
        {
            CirCleInterRegs.st_end_cir.x_st = PostionRegs.real_pos.pos_z; // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st = PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.x_end = EncodeRegs.AsReg.Z;
            CirCleInterRegs.st_end_cir.y_end = EncodeRegs.AsReg.X;
            CirCleInterRegs.st_end_cir.cen_x = EncodeRegs.AsReg.K + PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.cen_y = EncodeRegs.AsReg.I + PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.velo = EncodeRegs.AsReg.F;
        }
        if (EncodeRegs.AsReg.Plate == 19)
        {
            CirCleInterRegs.st_end_cir.x_st = PostionRegs.real_pos.pos_y; // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st = PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.x_end = EncodeRegs.AsReg.Y;
            CirCleInterRegs.st_end_cir.y_end = EncodeRegs.AsReg.Z;
            CirCleInterRegs.st_end_cir.cen_x = EncodeRegs.AsReg.J + PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.cen_y = EncodeRegs.AsReg.K + PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.velo = EncodeRegs.AsReg.F;
        }

        obj.reset();

        obj.in.startX = CirCleInterRegs.st_end_cir.x_st;
        obj.in.startY = CirCleInterRegs.st_end_cir.y_st;
        obj.in.endX = CirCleInterRegs.st_end_cir.x_end;
        obj.in.endY = CirCleInterRegs.st_end_cir.y_end;
        obj.in.cenX = CirCleInterRegs.st_end_cir.cen_x;
        obj.in.cenY = CirCleInterRegs.st_end_cir.cen_y;
        obj.in.dir = EncodeRegs.AsReg.Type; //   2/3;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed = (float)ThreeWordsReg.ChabuMoveSpeed / 9155 / 10000 * 60;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc = (float)ThreeWordsReg.VACSpeed / 9155 / 10000 * 60;
        obj.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed * g_params_vsr; //(100.0*60.0);
        obj.in.setA = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc;                      /// 100.0;//0.08;
        obj.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[0];                              /// 100.0;//0.08;
        obj.in.valid = 1;
        obj.sureInput();

        phase = 1;
        // RunstatusRegs.run_cnt=0;
    // no break;
    case 1:
        obj.run();
        PostionRegs.real_pos2.pos_x = obj.view.pos_x;
        PostionRegs.real_pos2.pos_y = obj.view.pos_y;

        if (obj.status == END)
        { // PostionRegs.real_pos2.pos_x=CirCleInterRegs.st_end_cir.x_end;
            // PostionRegs.real_pos2.pos_y=CirCleInterRegs.st_end_cir.y_end;
            RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
            RunstatusRegs.isCmdCompelete = 2;
            busy = 0;
            phase = 0;
        }
        else
        {
            if (cmdRun == 0) // need brake
            {
                obj.in.pauseCmd = 1;
                phase = 2;
            }
        }
        break;
    case 2:
        obj.run();
        PostionRegs.real_pos2.pos_x = obj.view.pos_x;
        PostionRegs.real_pos2.pos_y = obj.view.pos_y;

        if (obj.status == END)
        {
            phase = 3; // brake finished.
            busy = 0;  // means not busy, can do other things.
        }
        break;
    case 3: // ready restart
        if (cmdRun)
        {
            obj.in.pauseCmd = 0;
            obj.reStart();
            phase = 1;
        }
        break;
    default:
        phase = 0;
    }

    if (EncodeRegs.AsReg.Plate == 17)
    {
        PostionRegs.real_pos.pos_x = PostionRegs.real_pos2.pos_x;
        PostionRegs.real_pos.pos_y = PostionRegs.real_pos2.pos_y;
    }
    if (EncodeRegs.AsReg.Plate == 18)
    {
        PostionRegs.real_pos.pos_z = PostionRegs.real_pos2.pos_x;
        PostionRegs.real_pos.pos_x = PostionRegs.real_pos2.pos_y;
    }
    if (EncodeRegs.AsReg.Plate == 19)
    {
        PostionRegs.real_pos.pos_y = PostionRegs.real_pos2.pos_x;
        PostionRegs.real_pos.pos_z = PostionRegs.real_pos2.pos_y;
    }

    if (EncodeRegs.AsReg.Plate == 17 && (PostionRegs.real_pos.pos_x == EncodeRegs.AsReg.X) && (PostionRegs.real_pos.pos_y == EncodeRegs.AsReg.Y))
    {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
        CirCleInterRegs.cir_first = 0;
    }

    if (EncodeRegs.AsReg.Plate == 18 && (PostionRegs.real_pos.pos_z == EncodeRegs.AsReg.Z) && (PostionRegs.real_pos.pos_x == EncodeRegs.AsReg.X))
    {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
        CirCleInterRegs.cir_first = 0;
    }
    if (EncodeRegs.AsReg.Plate == 19 && (PostionRegs.real_pos.pos_y == EncodeRegs.AsReg.Y) && (PostionRegs.real_pos.pos_z == EncodeRegs.AsReg.Z))
    {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
        CirCleInterRegs.cir_first = 0;
    } // glh

    return busy;
}

/***
 * 圆弧插补算法
 */
uint16_t CircleInterp(uint16_t cmdRun, void *interpData)
{
    static Ccircle obj;
    static uint16_t phase = 0;
    uint16_t busy = 1;

    if (obj.status == END)
    { // if (cmdAutoReset==1)
      //   {   cmdAutoReset=0;
      //       phase=0;
      //   }
    }

    /* STM || LIMIT*/
    if (g_params_stm == 1 ||
        /* 两个运动轴都有限位说明两个轴都停止了 */
        (((InterplastLimit[g_params_plate_interp_axis[0] * 2] == 1) || (InterplastLimit[g_params_plate_interp_axis[0] * 2 + 1] == 1)) && ((InterplastLimit[g_params_plate_interp_axis[1] * 2] == 1) || (InterplastLimit[g_params_plate_interp_axis[1] * 2 + 1] == 1))))
    {
        phase = 0;
        // 轴全部停止则相当于：完成了这次插补运动并退出
        return 0;
    }

    switch (phase)
    {
    case 0:
    {
        /* 确定插补平面以及做速度规划 */
        int32_t px, py; /* 指定插补轴的位置 */
        px = *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]];
        py = *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]];

        InterpMotionBlock *block = (InterpMotionBlock *)interpData;

        CirCleInterRegs.st_end_cir.x_st = px; // 平面在此处加判断
        CirCleInterRegs.st_end_cir.y_st = py;
        CirCleInterRegs.st_end_cir.x_end = block->edx;
        CirCleInterRegs.st_end_cir.y_end = block->edy;
        //        CirCleInterRegs.st_end_cir.cen_x = block->i + px;
        //        CirCleInterRegs.st_end_cir.cen_y = block->j + py;
        CirCleInterRegs.st_end_cir.cen_x = block->i;
        CirCleInterRegs.st_end_cir.cen_y = block->j;
        CirCleInterRegs.st_end_cir.velo = block->vm;

        obj.reset();

        obj.in.startX = CirCleInterRegs.st_end_cir.x_st;
        obj.in.startY = CirCleInterRegs.st_end_cir.y_st;
        obj.in.endX = CirCleInterRegs.st_end_cir.x_end;
        obj.in.endY = CirCleInterRegs.st_end_cir.y_end;
        obj.in.cenX = CirCleInterRegs.st_end_cir.cen_x;
        obj.in.cenY = CirCleInterRegs.st_end_cir.cen_y;
        obj.in.dir = block->circle_type;                                    //   2/3;
                                                                            //        ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed = (float)ThreeWordsReg.ChabuMoveSpeed/9155/10000*60;
                                                                            //        ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc = (float)ThreeWordsReg.VACSpeed/9155/10000*60;
        obj.in.setV = (float)block->vm;                                     //(100.0*60.0);
        obj.in.setA = (float)block->am;                                     /// 100.0;//0.08;
        obj.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[0]; /// 100.0;//0.08;
        obj.in.valid = 1;
        obj.sureInput();

        phase = 1;
        // RunstatusRegs.run_cnt=0;
        // no break;
    }
    case 1:
        obj.run();
        PostionRegs.real_pos2.pos_x = obj.view.pos_x;
        PostionRegs.real_pos2.pos_y = obj.view.pos_y;

        if (obj.status == END)
        { // PostionRegs.real_pos2.pos_x=CirCleInterRegs.st_end_cir.x_end;
            // PostionRegs.real_pos2.pos_y=CirCleInterRegs.st_end_cir.y_end;
            RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
            RunstatusRegs.isCmdCompelete = 2;
            busy = 0;
            phase = 0;
        }
        else
        {
            if (cmdRun == 0) // need brake
            {
                obj.in.pauseCmd = 1;
                phase = 2;
            }
        }
        break;
    case 2:
        obj.run();
        PostionRegs.real_pos2.pos_x = obj.view.pos_x;
        PostionRegs.real_pos2.pos_y = obj.view.pos_y;

        if (obj.status == END)
        {
            phase = 3; // brake finished.
            busy = 0;  // means not busy, can do other things.
        }
        break;
    case 3: // ready restart
        if (cmdRun)
        {
            obj.in.pauseCmd = 0;
            obj.reStart();
            phase = 1;
        }
        break;
    default:
        phase = 0;
    }

    /* 更新位置 */
    //    *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]] = PostionRegs.real_pos2.pos_x;
    //    *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]] = PostionRegs.real_pos2.pos_y;

    /* 如果要输出正向脉冲并且当前没有正限位且没有STM停轴 || 如果要输出负向脉冲并且当前没有负限位且没有STM停轴 */
    //    if( (PostionRegs.real_pos2.pos_x - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]] > 0
    //            &&  nowIfLimit[g_params_plate_interp_axis[0] * 2] != 1
    //            && STMAxises[g_params_plate_interp_axis[0]] != 1) ||
    //        (PostionRegs.real_pos2.pos_x - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]] < 0
    //            &&  nowIfLimit[g_params_plate_interp_axis[0] * 2 + 1] != 1
    //            && STMAxises[g_params_plate_interp_axis[0]] != 1)){
    //        *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]] = PostionRegs.real_pos2.pos_x;
    //    }

    /* 如果要输出正向脉冲    并且当前没有正限位   且没有STM停轴  且上一次没有限位情况*/
    if (((PostionRegs.real_pos2.pos_x - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]] > 0) && (nowIfLimit[g_params_plate_interp_axis[0] * 2] != 1) && (STMAxises[g_params_plate_interp_axis[0]] != 1) && (InterplastLimit[g_params_plate_interp_axis[0] * 2] != 1))
        /* 或者 如果要输出负向脉冲    并且当前没有负限位   且没有STM停轴  且上一次没有限位情况*/
        || ((PostionRegs.real_pos2.pos_x - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]] < 0) && (nowIfLimit[g_params_plate_interp_axis[0] * 2 + 1] != 1) && (STMAxises[g_params_plate_interp_axis[0]] != 1) && (InterplastLimit[g_params_plate_interp_axis[0] * 2 + 1] != 1)))
    {
        *g_params_axisRealPositionPtr[g_params_plate_interp_axis[0]] = PostionRegs.real_pos2.pos_x;
    }
    else if (nowIfLimit[g_params_plate_interp_axis[0] * 2] == 1)
    {
        /* 保存上一次正限位情况  */
        InterplastLimit[g_params_plate_interp_axis[0] * 2] = 1;
    }
    else if (nowIfLimit[g_params_plate_interp_axis[0] * 2 + 1] == 1)
    {
        /* 保存上一次负限位情况  */
        InterplastLimit[g_params_plate_interp_axis[0] * 2 + 1] = 1;
    }

    //    if( (PostionRegs.real_pos2.pos_y - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]] > 0
    //            &&  nowIfLimit[g_params_plate_interp_axis[1] * 2] != 1
    //            && STMAxises[g_params_plate_interp_axis[1]] != 1) ||
    //        (PostionRegs.real_pos2.pos_y - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]] < 0
    //            &&  nowIfLimit[g_params_plate_interp_axis[1] * 2 + 1] != 1
    //            && STMAxises[g_params_plate_interp_axis[1]] != 1)){
    //        *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]] = PostionRegs.real_pos2.pos_y;
    //    }

    /* 如果要输出正向脉冲    并且当前没有正限位   且没有STM停轴  且上一次没有限位情况*/
    if (((PostionRegs.real_pos2.pos_y - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]] > 0) && (nowIfLimit[g_params_plate_interp_axis[1] * 2] != 1) && (STMAxises[g_params_plate_interp_axis[1]] != 1) && (InterplastLimit[g_params_plate_interp_axis[1] * 2] != 1))
        /* 或者 如果要输出负向脉冲    并且当前没有负限位   且没有STM停轴  且上一次没有限位情况*/
        || ((PostionRegs.real_pos2.pos_y - *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]] < 0) && (nowIfLimit[g_params_plate_interp_axis[1] * 2 + 1] != 1) && (STMAxises[g_params_plate_interp_axis[1]] != 1) && (InterplastLimit[g_params_plate_interp_axis[1] * 2 + 1] != 1)))
    {
        *g_params_axisRealPositionPtr[g_params_plate_interp_axis[1]] = PostionRegs.real_pos2.pos_y;
    }
    else if (nowIfLimit[g_params_plate_interp_axis[1] * 2] == 1)
    {
        /* 保存上一次正限位情况  */
        InterplastLimit[g_params_plate_interp_axis[1] * 2] = 1;
    }
    else if (nowIfLimit[g_params_plate_interp_axis[1] * 2 + 1] == 1)
    {
        /* 保存上一次负限位情况  */
        InterplastLimit[g_params_plate_interp_axis[1] * 2 + 1] = 1;
    }

    //    if (*g_params_axisPositionPtr[g_params_plate_interp_axis[0]] == EncodeRegs.AsReg.X &&
    //        *g_params_axisPositionPtr[g_params_plate_interp_axis[1]] == EncodeRegs.AsReg.Y)
    //    {
    //        RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
    //        CirCleInterRegs.cir_first=0;
    //    }
    /* 圆弧插补结束标志 */
    if (obj.status == END)
    {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
        CirCleInterRegs.cir_first = 0;
    }

    return busy;
}

//**********************************************************************
/* 旧版 弃用 */
uint16_t stopTable[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

uint16_t JOG(uint16_t Limit)
{
    static Cjog obj;
    static int phase = 0, axis = 0;
    uint16_t JogSel = JOGRegs.JogSel & 0xffff;
    uint16_t busy;

    switch (phase)
    {
    case 0:
        if (JogSel) //  (Alarm.limit.bit.xp == 1) &&(Alarm.softlimit.bit.xp==0)&&
        {
            for (axis = 0; axis < 16; axis++)
            {
                /* JogSel = 0x0001,axis = 0 */
                if (JogSel & ((uint16_t)1 << axis))
                    break;
            }

            obj.reset();
            obj.in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[axis >> 1]; ///(100.0*60.0);//  .003;

            if (MainAxisRegs.BeiLv == 10)
                obj.in.setV /= 10.0;
            else if (MainAxisRegs.BeiLv == 1)
                obj.in.setV /= 100.0;
            else                    // if (axis<2)
                obj.in.setV /= 5.0; // all axis not allow run fast

            if (obj.in.setV < 0.000002)
                obj.in.setV = 0.000002;
            obj.in.setAcc = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[axis >> 1]; /// 100.0;//0.08;

            if (obj.in.setJ < 0.02)
                obj.in.setJ = 0.02;
            obj.in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[axis >> 1]; /// 100.0;//0.08;
            obj.in.HiBrakeValue = 3;                                                    // ParamterUnRegs.ParamterRegs.AxisparmRegs.NSoftLimit[4]/1000;//test use

            /* 速度规划 */
            obj.sureInput();
            phase = 1;
            busy = 1;
        }
        else
            busy = 0;
        break;
    case 1:
        /* 执行加减速算法（加速阶段到匀速阶段），计算指令脉冲 */
        obj.runUp();

        switch (axis)
        {
        case 0:
            PostionRegs.real_pos.pos_x += obj.view.delta;
            break;
        case 1:
            PostionRegs.real_pos.pos_x -= obj.view.delta;
            break;
        case 2:
            PostionRegs.real_pos.pos_y += obj.view.delta;
            break;
        case 3:
            PostionRegs.real_pos.pos_y -= obj.view.delta;
            break;
        case 4:
            PostionRegs.real_pos.pos_z += obj.view.delta;
            break;
        case 5:
            PostionRegs.real_pos.pos_z -= obj.view.delta;
            break;
        case 6:
            PostionRegs.real_pos.pos_a += obj.view.delta;
            break;
        case 7:
            PostionRegs.real_pos.pos_a -= obj.view.delta;
            break;
        case 8:
            PostionRegs.real_pos.pos_b += obj.view.delta;
            break;
        case 9:
            PostionRegs.real_pos.pos_b -= obj.view.delta;
            break;
        case 10:
            PostionRegs.real_pos.pos_c += obj.view.delta;
            break;
        case 11:
            PostionRegs.real_pos.pos_c -= obj.view.delta;
            break;
        case 12:
            PostionRegs.real_pos.pos_w += obj.view.delta;
            break;
        case 13:
            PostionRegs.real_pos.pos_w -= obj.view.delta;
            break;
        case 14:
            PostionRegs.real_pos.pos_u += obj.view.delta;
            break;
        case 15:
            PostionRegs.real_pos.pos_u -= obj.view.delta;
            break;
        }

        if ((JogSel & ((uint16_t)1 << axis)) == 0)
        {
            uint16_t temp;

            //				temp=stopTable[axis];
            //				if(   (~glAxisLimit) & ((uint16_t)1<<temp) )//means Jog meet limit
            //														 need stop
            //				{	phase=3;
            //				}
            //				else
            {
                obj.calBrake();
                phase = 2;
            }
        }
        busy = 1;

        break;
    case 2:
        /* 刹车 */
        obj.runDown();
        switch (axis)
        {
        case 0:
            PostionRegs.real_pos.pos_x += obj.view.delta;
            break;
        case 1:
            PostionRegs.real_pos.pos_x -= obj.view.delta;
            break;
        case 2:
            PostionRegs.real_pos.pos_y += obj.view.delta;
            break;
        case 3:
            PostionRegs.real_pos.pos_y -= obj.view.delta;
            break;
        case 4:
            PostionRegs.real_pos.pos_z += obj.view.delta;
            break;
        case 5:
            PostionRegs.real_pos.pos_z -= obj.view.delta;
            break;
        case 6:
            PostionRegs.real_pos.pos_a += obj.view.delta;
            break;
        case 7:
            PostionRegs.real_pos.pos_a -= obj.view.delta;
            break;
        case 8:
            PostionRegs.real_pos.pos_b += obj.view.delta;
            break;
        case 9:
            PostionRegs.real_pos.pos_b -= obj.view.delta;
            break;
        case 10:
            PostionRegs.real_pos.pos_c += obj.view.delta;
            break;
        case 11:
            PostionRegs.real_pos.pos_c -= obj.view.delta;
            break;
        case 12:
            PostionRegs.real_pos.pos_w += obj.view.delta;
            break;
        case 13:
            PostionRegs.real_pos.pos_w -= obj.view.delta;
            break;
        case 14:
            PostionRegs.real_pos.pos_u += obj.view.delta;
            break;
        case 15:
            PostionRegs.real_pos.pos_u -= obj.view.delta;
            break;
        }

        {
            uint16_t temp;

            temp = stopTable[axis]; //

            if (obj.status == END)
            {
                phase = 0;
                busy = 0;
            }
            else if ((~glAxisLimit) & ((uint16_t)1 << temp)) // means Jog meet limit
            {
                busy = 1;
                obj.in.cmdHiBrake = 1; // need stop
                                       // phase=3;
            }
            else
            {
                busy = 1;
            }
        }
        break;
    case 3:
        phase = 0;
        busy = 0;
        break;
    default:
        phase = 0;
    }
    return busy;
}

/* 8轴JOG版  2023.07.24*/
uint16_t MyJOG2()
{
    static Cjog obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8;
    static Cjog obj[8] = {obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8}; // 8个轴jog
    int m;
    /* JOG轴 */
    /* jogAxis = 0000 0000 0000 1001,代表A轴正B轴负转 */
    uint16_t JogSel = jogAxis & 0xffff;
    static int JogPhases[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    /* 急停 */
    if (((g_io_nlimitIO) & (1 << 8)) != 0)
    {
        g_params_jog_enable = 0;
        return 0;
    }

    /* 如果有新的JOG指令 */
    /* 只有当前运动轴全部停下后才能进行新的JOG指令 */
    /* 当前运动的轴必须都进行刹车操作 */
    if (NewJogCommand == 1 && AllAxisEnd == 0xffff)
    {
        /* 重新初始化axises */
        for (m = 0; m < 15; m++)
        {
            axises[m] = 0;
        }
        /* 指定哪些轴还需要jog */
        int axis = 0;
        while (axis < 16)
        {
            if (JogSel & ((uint16_t)1 << axis))
            {
                axises[axis] = 1;
            }
            axis++;
        }
        /* 初始化每个轴的流程 */
        for (m = 0; m < 8; m++)
        {
            JogPhases[m] = 0;
        }
    }

    int AxisID;
    for (AxisID = 0; AxisID < 8; AxisID++)
    {
        /* 对于每个轴都有一个情况  */
        switch (JogPhases[AxisID])
        {
        case 0:
            /* 如果axises里偶数开始相邻两个有一个为1， 则代表需要jog*/
            if (axises[2 * AxisID] == 1 || axises[AxisID * 2 + 1] == 1)
            {
                /* 对当前轴进行速度加速度等参数的设置 */
                obj[AxisID].reset();
                obj[AxisID].in.setV = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[AxisID]; ///(100.0*60.0);//  .003;
                if (obj[AxisID].in.setV < 0)
                {
                    obj[AxisID].in.setV = -obj[AxisID].in.setV;
                }
                obj[AxisID].in.setAcc = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[AxisID]; /// 100.0;//0.08;
                obj[AxisID].in.setJ = (float)ParamterUnRegs.ParamterRegs.AxisparmRegs.J[AxisID];         /// 100.0;//0.08;
                if (obj[AxisID].in.setJ < 0.02)
                    obj[AxisID].in.setJ = 0.02;
                obj[AxisID].in.HiBrakeValue = 3; // ParamterUnRegs.ParamterRegs.AxisparmRegs.NSoftLimit[4]/1000;//test use
                /* 速度规划 */
                obj[AxisID].sureInput();
                JogPhases[AxisID] = 1;
            }
            /*  如果所有轴都停止了，且新指令执行完毕 */
            if (AxisID == 7 && NewJogCommand == 1 && AllAxisEnd == 0xffff)
            {
                NewJogCommand = 0;
            }
            break;
        case 1:
            /* 执行加减速算法（加速阶段到匀速阶段），计算指令脉冲 */
            /* 如果axises里偶数开始相邻两个有一个为1， 则代表需要jog*/
            if (axises[2 * AxisID] == 1 || axises[AxisID * 2 + 1] == 1)
            {
                obj[AxisID].runUp();
                /* 判断到底是正转还是反转 */
                /* 正转 */
                if (axises[2 * AxisID] == 1)
                {
                    /* 如果正转正限位 或者 停轴  或者有新指令进来了*/
                    if (nowIfLimit[AxisID * 2] == 1 || STMAxises[AxisID] == 1 || NewJogCommand == 1 ||
                        (((fsw_enable_flag & (1 << AxisID)) != 0) && (((g_io_plimitIO & (1 << AxisID)) != 0))) ||                                            // 这个判断条件是fsw指令使能了并且对应轴的回零开关打开了，该轴就会停止运动
                        (((fid_enable_flag & (1 << AxisID)) != 0) && (((z_pulse_param_buf & (1 << AxisID)) != 0)) && ((g_io_plimitIO & (1 << AxisID)) == 0)) // 这个判断条件是fid指令使能了并且对应轴的z脉冲检测到了（此时离开了对应的回零开关），该轴就会停止运动
                    )
                    {
                        obj[AxisID].calBrake();
                        /* 需要刹车进入下一阶段 */
                        JogPhases[AxisID] = 2;
                    }
                    else
                    {
                        /* 如果是负限位 或者没有正限位或者 是其他轴限位，可以输出脉冲*/
                        *g_params_axisRealPositionPtr[AxisID] += obj[AxisID].view.delta;
                    }
                }
                /* 反转 */
                else
                {
                    /* 如果反转反限位 或者 停轴  或者有新指令进来了*/
                    if (nowIfLimit[AxisID * 2 + 1] == 1 || STMAxises[AxisID] == 1 || NewJogCommand == 1 ||
                        (((fsw_enable_flag & (1 << AxisID)) != 0) && (((g_io_plimitIO & (1 << AxisID)) != 0))) ||                                            // 这个判断条件是fsw指令使能了并且对应轴的回零开关打开了，该轴就会停止运动
                        (((fid_enable_flag & (1 << AxisID)) != 0) && (((z_pulse_param_buf & (1 << AxisID)) != 0)) && ((g_io_plimitIO & (1 << AxisID)) == 0)) // 这个判断条件是fid指令使能了并且对应轴的z脉冲检测到了（此时离开了对应的回零开关），该轴就会停止运动

                    )
                    {
                        obj[AxisID].calBrake();
                        /* 需要刹车进入下一阶段 */
                        JogPhases[AxisID] = 2;
                    }
                    else
                    {
                        /* 如果是负限位 或者没有正限位或者 是其他轴限位，可以输出脉冲*/
                        *g_params_axisRealPositionPtr[AxisID] -= obj[AxisID].view.delta;
                    }
                }
            }
            break;
        case 2:
            if (axises[2 * AxisID] == 1 || axises[AxisID * 2 + 1] == 1)
            {
                /* 刹车 */
                obj[AxisID].runDown();
                /* 正转 */
                if (axises[2 * AxisID] == 1)
                {
                    *g_params_axisRealPositionPtr[AxisID] += obj[AxisID].view.delta;
                }
                /* 反转 */
                else
                {
                    *g_params_axisRealPositionPtr[AxisID] -= obj[AxisID].view.delta;
                }

                if (obj[AxisID].status == END)
                {
                    /* 如果是限位开关或者限位导致的，或者是STM导致的，该轴停下后不准运动 */
                    axises[2 * AxisID] = 0;
                    axises[2 * AxisID + 1] = 0;
                    /* 记录当前已经停下的轴 */
                    AllAxisEnd |= (1 << AxisID);
                    // fsw和fid使能复位
                    if (((fsw_enable_flag & (1 << AxisID)) != 0) && (((g_io_plimitIO & (1 << AxisID)) != 0)))
                    { // 这个判断条件是fsw指令使能了并且对应轴的回零开关打开了，该轴就会停止运动，此时需要复位
                        fsw_enable_flag &= ~(1 << AxisID);
                    }
                    if (((fid_enable_flag & (1 << AxisID)) != 0) && (((z_pulse_param_buf & (1 << AxisID)) != 0)))
                    {
                        //                        if ( ( (fid_enable_flag & (1 << AxisID)) != 0 ) && ( ((z_pulse_param_buf & (1 << AxisID)) != 0) ) && ( (g_io_plimitIO & (1 << AxisID)) == 0 ) ) {  //这个判断条件是fid指令使能了并且对应轴的z脉冲检测到了（此时离开了对应的回零开关），该轴就会停止运动，此时需要复位
                        fid_enable_flag &= ~(1 << AxisID);
                        z_pulse_param_buf &= ~(1 << AxisID);
                    }

                    /* 停下后回到第一步 */
                    JogPhases[AxisID] = 0;
                }
                else if (obj[AxisID].status != END)
                {
                    /* 记录当前还没停的轴 */
                    AllAxisEnd &= ~(1 << AxisID);
                    /* 还没停需要高级制动 */
                    obj[AxisID].in.cmdHiBrake = 1;
                }
            }
            break;
        default:
            JogPhases[AxisID] = 0;
        }
    }

    return 1;
}

//******************************************************
uint16_t RetRefPoint(void)
{
    extern uint16_t glAxisLimit;
    extern uint16_t refRun, glZE, glZeroPosi;

    static uint8_t phaseX = 0, phaseY = 0, phaseZ = 0, phaseA = 0, phaseB = 0;
    // static uint8_t zeroX=0,indexX=0;
    static CreturnRef objX, objY, objZ, objA, objB;
    static long bakX, bakY, bakZ, bakA, bakB;
    // if(RunstatusRegs.ReturnFlag.bit.x == 1)

    //------------------------ X axis --------------------------------------------
    switch (phaseX)
    {
    case 0:
        if (refRun == 1)
        {
            if (RunstatusRegs.ReturnFlag.bit.x == 1)
            {
                objX.reset();
                objX.in.setVrapid = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[0] * 0.5;
                objX.in.setVslow = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[0] * 0.2;
                objX.in.setAcc = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[0] * 0.2;        //
                objX.in.ZeroShift = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroShift[0] * 0.001; // unit is m
                if (ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroDir[0])
                    objX.in.dir = 1;
                else
                    objX.in.dir = -1;
                objX.sureInput();
                bakX = PostionRegs.real_pos.pos_x;
                phaseX = 1;
            }
        }
        break;
    case 1:
    {
        uint16_t zero, index;

        // zero=(~glAxisLimit) & 0x1;
        zero = (~glZeroPosi) & 0x1; //(~glAxisLimit) & 0x1; //!GpioG2DataRegs.AIODAT.bit.AIO2;
        index = glZE & 0x1;         //! GpioG2DataRegs.AIODAT.bit.AIO4;
        objX.run(zero, index);
    }
        PostionRegs.real_pos.pos_x = bakX + objX.view.position;
        if (objX.status == END)
        {
            PostionRegs.real_pos.pos_x = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[0]; // 当前坐标设置为参考点预设坐标
            PostionRegs.real_posbk.pos_x = PostionRegs.real_pos.pos_x;
            //	RunstatusRegs.ReturnFlag.bit.x = 0;
            phaseX = 2;
        }
        else if (refRun == 0)
            phaseX = 0;

        break;
    case 2:
        if (refRun == 0)
            phaseX = 0;
        break;
    default:
        phaseX = 0;
    }
    //--------------------------- y axis  -----------------------------------------
    switch (phaseY)
    {
    case 0:
        if (refRun == 1)
        {
            if (RunstatusRegs.ReturnFlag.bit.y == 1)
            {
                objY.reset();
                objY.in.setVrapid = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[1];
                objY.in.setVslow = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[1] * 0.2;
                objY.in.setAcc = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[1]; //
                objY.in.ZeroShift = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroShift[1] * 0.001;
                if (ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroDir[1])
                    objY.in.dir = 1;
                else
                    objY.in.dir = -1;
                objY.sureInput();
                bakY = PostionRegs.real_pos.pos_y;
                phaseY = 1;
            }
        }
        break;
    case 1:
    {
        uint16_t zero, index;

        // zero=(~glAxisLimit) & 0x4;
        zero = (~glZeroPosi) & 0x2; //(~glAxisLimit) & 0x1; //!GpioG2DataRegs.AIODAT.bit.AIO2;
        index = glZE & 0x2;         //! GpioG2DataRegs.AIODAT.bit.AIO4;
        objY.run(zero, index);
    }
        PostionRegs.real_pos.pos_y = bakY + objY.view.position;
        if (objY.status == END)
        {
            PostionRegs.real_pos.pos_y = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[1]; // 当前坐标设置为参考点预设坐标
            PostionRegs.real_posbk.pos_y = PostionRegs.real_pos.pos_y;

            phaseY = 2;
            //	RunstatusRegs.ReturnFlag.bit.x = 0;
        }
        else if (refRun == 0)
            phaseY = 0;

        break;
    case 2:
        if (refRun == 0)
            phaseY = 0;
        break;
    default:
        phaseY = 0;
    }
    //--------------------------- z axis  -----------------------------------------
    switch (phaseZ)
    {
    case 0:
        if (refRun == 1)
        {
            if (RunstatusRegs.ReturnFlag.bit.z == 1)
            {
                objZ.reset();
                objZ.in.setVrapid = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[2];
                objZ.in.setVslow = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[2] * 0.2;
                objZ.in.setAcc = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[2]; //
                objZ.in.ZeroShift = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroShift[2] * 0.001;
                if (ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroDir[2])
                    objZ.in.dir = 1;
                else
                    objZ.in.dir = -1;
                objZ.sureInput();
                bakZ = PostionRegs.real_pos.pos_z;
                phaseZ = 1;
            }
        }
        break;
    case 1:
    {
        uint16_t zero, index;

        // zero=(~glAxisLimit) & 0x10;
        zero = (~glZeroPosi) & 0x4; //(~glAxisLimit) & 0x1; //!GpioG2DataRegs.AIODAT.bit.AIO2;
        index = glZE & 0x4;         //! GpioG2DataRegs.AIODAT.bit.AIO4;
        objZ.run(zero, index);
    }
        PostionRegs.real_pos.pos_z = bakZ + objZ.view.position;
        if (objZ.status == END)
        {
            PostionRegs.real_pos.pos_z = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[2]; // 当前坐标设置为参考点预设坐标
            PostionRegs.real_posbk.pos_z = PostionRegs.real_pos.pos_z;

            phaseZ = 2;
            //	RunstatusRegs.ReturnFlag.bit.x = 0;
        }
        else if (refRun == 0)
            phaseZ = 0;

        break;
    case 2:
        if (refRun == 0)
            phaseZ = 0;
        break;
    default:
        phaseZ = 0;
    }
    //--------------------------- a axis  -----------------------------------------
    switch (phaseA)
    {
    case 0:
        if (refRun == 1)
        {
            if (RunstatusRegs.ReturnFlag.bit.a == 1) //
            {
                objA.reset();
                objA.in.setVrapid = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[3];
                objA.in.setVslow = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[3] * 0.2;
                objA.in.setAcc = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[3]; //
                objA.in.ZeroShift = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroShift[3] * 0.001;
                if (ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroDir[3])
                    objA.in.dir = 1;
                else
                    objA.in.dir = -1;
                objA.sureInput();
                bakA = PostionRegs.real_pos.pos_a;
                phaseA = 1;
            }
        }
        break;
    case 1:
    {
        uint16_t zero, index;
        // zero=(~glAxisLimit) & 0x40;
        zero = (~glZeroPosi) & 0x8; //(~glAxisLimit) & 0x1; //!GpioG2DataRegs.AIODAT.bit.AIO2;
        index = glZE & 0x8;         //! GpioG2DataRegs.AIODAT.bit.AIO4;
        objA.run(zero, index);
    }
        PostionRegs.real_pos.pos_a = bakA + objA.view.position;
        if (objA.status == END)
        {
            PostionRegs.real_pos.pos_a = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[3]; // 当前坐标设置为参考点预设坐标
            PostionRegs.real_posbk.pos_a = PostionRegs.real_pos.pos_a;
            phaseA = 2;
            //	RunstatusRegs.ReturnFlag.bit.x = 0;
        }
        else if (refRun == 0)
            phaseA = 0;

        break;
    case 2:
        if (refRun == 0)
            phaseA = 0;
        break;
    default:
        phaseA = 0;
    }

    //--------------------------- b axis  -----------------------------------------
    switch (phaseB)
    {
    case 0:
        if (refRun == 1)
        {
            if (RunstatusRegs.ReturnFlag.bit.b == 1) // b
            {
                objB.reset();
                objB.in.setVrapid = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[4];
                objB.in.setVslow = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroSpeed[4] * 0.2;
                objB.in.setAcc = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[4]; //
                objB.in.ZeroShift = ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroShift[4] * 0.001;
                if (ParamterUnRegs.ParamterRegs.AxisparmRegs.ZeroDir[4])
                    objB.in.dir = 1;
                else
                    objB.in.dir = -1;
                objB.sureInput();
                bakB = PostionRegs.real_pos.pos_b;
                phaseB = 1;
            }
        }
        break;
    case 1:
    {
        uint16_t zero, index;

        // zero=(~glAxisLimit) & 0x100;
        zero = (~glZeroPosi) & 0x10; //(~glAxisLimit) & 0x1; //!GpioG2DataRegs.AIODAT.bit.AIO2;
        index = glZE & 0x10;         //! GpioG2DataRegs.AIODAT.bit.AIO4;
        objB.run(zero, index);
    }
        PostionRegs.real_pos.pos_b = bakB + objB.view.position;
        if (objB.status == END)
        {
            PostionRegs.real_pos.pos_b = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[4]; // 当前坐标设置为参考点预设坐标
            PostionRegs.real_posbk.pos_b = PostionRegs.real_pos.pos_b;

            phaseB = 2;
            //	RunstatusRegs.ReturnFlag.bit.x = 0;
        }
        else if (refRun == 0)
            phaseB = 0;

        break;
    case 2:
        if (refRun == 0)
            phaseB = 0;
        break;
    default:
        phaseB = 0;
    }

    //----------------------------------------------------------------------------------
    if ((objX.status == BUSY) || (objY.status == BUSY) || (objZ.status == BUSY) || (objZ.status == BUSY) || (objA.status == BUSY) || (objB.status == BUSY))
        RunstatusRegs.RunStatusFlag.bit.Ret_Flag = 0; // means finished return ref
    else
        RunstatusRegs.RunStatusFlag.bit.Ret_Flag = 1; // means finished return ref
                                                      //  RunstatusRegs.RunStatusFlag==PLC 224
    return 0;
}
//**********************************************************************
