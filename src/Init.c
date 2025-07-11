#include "variabledef.h"
#include <string.h>
// 参数的初始化
#define GX1DEFAULT 0  // G04,G10,G27-G31,G36,G37,G50,G70-G74,G76,G80-G89,G98,G99
#define GX2DEFAULT 0  // G00,G01,G02,G03,G32,G33,G100
#define GX3DEFAULT 0  // G40-G49
#define GX4DEFAULT 54 // G51-G59
#define GX5DEFAULT 90 // G90,G91
#define GX6DEFAULT 0  // G20,G21
#define GX7DEFAULT 17 // G17,G18,G19
#define GX8DEFAULT 0  // G11,G12
#define WTYPEDEFAULT 0
#define SPEEDEFAULT 100 // default:100
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// 函数功能：   初始化参数
// 输入参数：   无..
// 输出参数：   无
// 创建时间：   2005.11.14
// 最后修改时间:2006.02.07
// 修改原因：   GX1---GX7的默认数据进行修改
// 创建人  ：   杨晓龙
//---------------------------------------------------------------------------
// extern struct inter_pos RC_bak;
extern int32_t Out1bk, Out2bk, Out3bk, Out4bk, Out5bk, Out6bk;
extern uint16_t SpeedDown;
extern int S_speed;
struct RPDAccDECST RPDAccDecRegs;
struct BPMParams bpmParams;
struct RunStatusST RunstatusRegs;
struct ThreeWordsST ThreeWordsReg;
uint16_t CNCCodeChar[CNCCODELength];

void InitPara(void)
{
    // memcpy(&bpmParams,(BPMParams *) Bzero_Sector9_start, sizeof(BPMParams));
    int i;
    // 初始化Cs寄存器的内容
    //   EncodeRegs.CsReg.N   =0;                   // 未使用
    EncodeRegs.CsReg.GX1 = GX1DEFAULT; // 基代码分组
    EncodeRegs.CsReg.GX2 = GX2DEFAULT;
    EncodeRegs.CsReg.GX3 = GX3DEFAULT;
    EncodeRegs.CsReg.GX4 = GX4DEFAULT;
    EncodeRegs.CsReg.GX5 = GX5DEFAULT;
    EncodeRegs.CsReg.GX6 = GX6DEFAULT;
    EncodeRegs.CsReg.GX7 = GX7DEFAULT;
    EncodeRegs.CsReg.GX8 = GX8DEFAULT;
    EncodeRegs.CsReg.X = 0;           // X轴坐标
    EncodeRegs.CsReg.Y = 0;           // Y轴坐标
    EncodeRegs.CsReg.Z = 0;           // Z轴坐标
    EncodeRegs.CsReg.A = 0;           // A轴坐标
    EncodeRegs.CsReg.B = 0;           // B轴坐标
    EncodeRegs.CsReg.C = 0;           // C轴坐标
    EncodeRegs.CsReg.W = 0;           // B轴坐标
    EncodeRegs.CsReg.U = 0;           // C轴坐标
    EncodeRegs.CsReg.I = 0;           // 圆弧插补 X轴圆心-起点
    EncodeRegs.CsReg.J = 0;           // 圆弧插补 Y轴圆心-起点
    EncodeRegs.CsReg.K = 0;           // 圆弧插补 Z轴圆心-起点
    EncodeRegs.CsReg.F = SPEEDEFAULT; // 进给速度
    EncodeRegs.CsReg.FZ = 0;          // 螺旋线插补垂直轴速度
    EncodeRegs.CsReg.H = 0;
    EncodeRegs.CsReg.M = 0;    // M代码
    EncodeRegs.CsReg.M1 = 100; // M代码
    EncodeRegs.CsReg.M2 = 5;
    EncodeRegs.CsReg.M3 = 100;
    EncodeRegs.CsReg.M4 = 0; // M08 开冷却液      M09关冷却液
    EncodeRegs.CsReg.M5 = 0; // M32 润滑开       M33润滑关
    EncodeRegs.CsReg.M6 = 0;
    EncodeRegs.CsReg.M7 = 0;
    EncodeRegs.CsReg.M8 = 0;
    EncodeRegs.CsReg.M9 = 0;
    EncodeRegs.CsReg.M10 = 0;
    EncodeRegs.CsReg.T = 0; // T代码
    EncodeRegs.CsReg.R = 0; // 圆弧半径
    EncodeRegs.CsReg.P = 0; // G04等待时间
    EncodeRegs.CsReg.Flag = 0;

    EncodeRegs.CsReg.D = 0;
    EncodeRegs.CsReg.E = 0;
    EncodeRegs.CsReg.N = 0;
    EncodeRegs.CsReg.O = 0;
    EncodeRegs.CsReg.Q = 0;
    EncodeRegs.CsReg.V = 0;
    EncodeRegs.CsReg.N7 = 0;
    EncodeRegs.CsReg.N8 = 0;
    EncodeRegs.CsReg.MotorEnable = 255;

    // 初始化备份的Cs寄存器的内容
    // CsRegBK.N   =0;
    EncodeRegs.CsRegBK = EncodeRegs.CsReg;
    // 初始化CsNext寄存器的内容
    EncodeRegs.CsNext = EncodeRegs.CsReg;
    // 初始化Bs寄存器的内容
    EncodeRegs.BsReg = EncodeRegs.CsReg;
    // 初始化As寄存器的内容
    // 可以不进行初始化As的内容，直接从Bs中进行传递即可
    EncodeRegs.AsReg.Type = WTYPEDEFAULT; // 工作方式标志
    EncodeRegs.AsReg.X = 0;               // 各轴坐标
    EncodeRegs.AsReg.Y = 0;
    EncodeRegs.AsReg.Z = 0;
    EncodeRegs.AsReg.A = 0;
    EncodeRegs.AsReg.B = 0;
    EncodeRegs.AsReg.C = 0;
    EncodeRegs.AsReg.W = 0;
    EncodeRegs.AsReg.U = 0;

    EncodeRegs.AsReg.I = 0; // 圆弧插补 圆心-起点
    EncodeRegs.AsReg.J = 0;
    EncodeRegs.AsReg.K = 0;
    EncodeRegs.AsReg.P = 0;           // G04等待时间
    EncodeRegs.AsReg.F = SPEEDEFAULT; // 进给速度
    EncodeRegs.CsReg.FZ = 0;          // 螺旋线插补垂直轴速度
    EncodeRegs.AsReg.M = 0;           // M代码
    EncodeRegs.AsReg.M1 = 100;
    EncodeRegs.AsReg.M2 = 100;
    EncodeRegs.AsReg.M3 = 100;
    EncodeRegs.AsReg.Co = 54;   // 坐标系
    EncodeRegs.AsReg.Abs = 0;   //
    EncodeRegs.AsReg.Plate = 0; // 平面选择
    EncodeRegs.AsReg.Flag = 0;

    EncodeRegs.AsReg.D = 0;
    EncodeRegs.AsReg.E = 0;
    EncodeRegs.AsReg.N = 0;
    EncodeRegs.AsReg.O = 0;
    EncodeRegs.AsReg.Q = 0;
    EncodeRegs.AsReg.V = 0;
    EncodeRegs.AsReg.N7 = 0;
    EncodeRegs.AsReg.N8 = 0;
    EncodeRegs.AsReg.MotorEnable = 255;
    // 初始化内部代码缓冲区的地址
    EncodeRegs.IncodeAdd = (uint16_t *)&CNCCodeChar[0];
    // 初始化工作模式参数
    RunstatusRegs.RunStatusFlag.bit.Type = 0;
    // 流程控制
    RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
    // 内部代码缓冲区标志
    EncodeRegs.IncodeFlag = 0;
    // 快速定位控制标志
    RunstatusRegs.RpdFlag = 0;

    //  count_speed_min = 0; //chenlin 2014.1.16
    RunstatusRegs.circle_MDIstop_flag = 0;
    // 相对位置的初始化
    //   PostionRegs.AbsPos.pos_x = 0;
    //   PostionRegs.AbsPos.pos_y = 0;
    //   PostionRegs.AbsPos.pos_z = 0;
    //   PostionRegs.AbsPos.pos_a = 0;
    //   PostionRegs.AbsPos.pos_b = 0;
    //   PostionRegs.AbsPos.pos_c = 0;
    // 零件坐标
    PostionRegs.LinPos = PostionRegs.AbsPos;

    // 坐标选择
    RunstatusRegs.CorpSel = 0;
    // JOG
    JOGRegs.JogSel = 0;
    // 81000
    RunstatusRegs.RunStatusFlag.bit.AutoJud = 0;
    // 倍率
    MainAxisRegs.BeiLv = 1; // default 1
    // 点动轴选择
    RunstatusRegs.DdSel = 0;
    RunstatusRegs.DdSelBk = 0;
    // 螺旋线初始化一次的变量
    //  CylinderFlag = 0;

    //     PostionRegs.AbsPos.pos_x = 0;
    //     PostionRegs.AbsPos.pos_y = 0;
    //     PostionRegs.AbsPos.pos_z = 0;
    //   PostionRegs.AbsPos.pos_a = 0;//chenlin2014.3.10
    //     PostionRegs.AbsPos.pos_b = 0;
    //     PostionRegs.AbsPos.pos_c = 0;
    // 前次位置信息
    // PostionRegs.last_real_posbk
    PostionRegs.real_posbk = PostionRegs.AbsPos;
    // 输出脉冲
    PostionRegs.out_imp = PostionRegs.AbsPos;
    // 插补变量初始化
    RunstatusRegs.st_end.x_st = 0;
    RunstatusRegs.st_end.y_st = 0;
    RunstatusRegs.st_end.z_st = 0;
    RunstatusRegs.st_end.a_st = 0;
    RunstatusRegs.st_end.b_st = 0;
    RunstatusRegs.st_end.c_st = 0;
    RunstatusRegs.st_end.w_st = 0;
    RunstatusRegs.st_end.u_st = 0;
    RunstatusRegs.st_end.x_end = 0;
    RunstatusRegs.st_end.y_end = 0;
    RunstatusRegs.st_end.z_end = 0;
    RunstatusRegs.st_end.a_end = 0;
    RunstatusRegs.st_end.b_end = 0;
    RunstatusRegs.st_end.c_end = 0;
    RunstatusRegs.st_end.w_end = 0;
    RunstatusRegs.st_end.u_end = 0;
    RunstatusRegs.st_end.velo = 0;
    LineInterRegs.velo_step.velo_x = 0;
    LineInterRegs.velo_step.velo_y = 0;
    LineInterRegs.velo_step.velo_z = 0;
    LineInterRegs.velo_step.velo_a = 0;
    LineInterRegs.velo_step.velo_b = 0;
    LineInterRegs.velo_step.velo_c = 0;
    LineInterRegs.velo_step.velo_w = 0;
    LineInterRegs.velo_step.velo_u = 0;
    PostionRegs.real_pos = PostionRegs.AbsPos;
    PostionRegs.real_pos1 = PostionRegs.AbsPos;
    PostionRegs.real_pos2 = PostionRegs.AbsPos;
    PostionRegs.real_pos3 = PostionRegs.AbsPos;
    LineInterRegs.velo_add = 0; // 步长累加器
    CirCleInterRegs.st_end_cir.x_st = 0;
    CirCleInterRegs.st_end_cir.y_st = 0;
    CirCleInterRegs.st_end_cir.x_end = 0;
    CirCleInterRegs.st_end_cir.y_end = 0;
    CirCleInterRegs.st_end_cir.cen_x = 0;
    CirCleInterRegs.st_end_cir.cen_y = 0;
    CirCleInterRegs.st_end_cir.cir_r = 0;
    CirCleInterRegs.st_end_cir.velo = 0;
    CirCleInterRegs.remx = 0, CirCleInterRegs.remy = 0, CirCleInterRegs.remx1 = 0, CirCleInterRegs.remy1 = 0;
    CirCleInterRegs.half_length.int_h = 0;
    CirCleInterRegs.half_length.int_l = 0;
    CirCleInterRegs.cir_half = 0, CirCleInterRegs.cir_flag = 0;
    CirCleInterRegs.zuo_you = 0;
    RunstatusRegs.run_cnt = 0;
    RunstatusRegs.Angle_cnt = 0;
    LineInterRegs.lin_remx = 0;
    LineInterRegs.lin_remy = 0;
    LineInterRegs.lin_remz = 0;
    LineInterRegs.lin_rema = 0;
    LineInterRegs.lin_remb = 0;
    LineInterRegs.lin_remc = 0;
    LineInterRegs.lin_remw = 0;
    LineInterRegs.lin_remu = 0;
    LineInterRegs.add_x = 0;
    LineInterRegs.add_y = 0;
    LineInterRegs.add_z = 0;
    LineInterRegs.add_a = 0;
    LineInterRegs.add_b = 0;
    LineInterRegs.add_c = 0;
    LineInterRegs.add_w = 0;
    LineInterRegs.add_u = 0;
    //   real_x_bak=0;
    //   real_y_bak=0;
    //   PTNum =0;
    //  G28Flag = 0 ;
    RunstatusRegs.ret_finished_x = 0; // 回参完成标志
    RunstatusRegs.ret_finished_y = 0;
    RunstatusRegs.ret_finished_z = 0;
    RunstatusRegs.ret_finished_a = 0;
    RunstatusRegs.ret_finished_b = 0;
    RunstatusRegs.ret_finished_c = 0;
    RunstatusRegs.ret_finished_w = 0;
    RunstatusRegs.ret_finished_u = 0;

    /*  dir_comp.x = 0;     // 方向调整完成标志
        dir_comp.y = 0;
        dir_comp.z = 0;
        dir_comp.a = 0;
        dir_comp.b = 0;
        dir_comp.c = 0;

        low_speed.x = 0;    // 低速状态标志
        low_speed.y = 0;
        low_speed.z = 0;
        low_speed.a = 0;
        low_speed.b = 0;
        low_speed.c = 0;

        slow_bak.x = 0;     // 上次减速信号
        slow_bak.y = 0;
        slow_bak.z = 0;
        slow_bak.a = 0;
        slow_bak.b = 0;
        slow_bak.c = 0;
    */
    PostionRegs.offset_temp.pos_x = 5000; // 偏置距离临时值
    PostionRegs.offset_temp.pos_y = 5000;
    PostionRegs.offset_temp.pos_z = 5000;
    PostionRegs.offset_temp.pos_a = 5000;
    PostionRegs.offset_temp.pos_b = 5000;
    PostionRegs.offset_temp.pos_c = 5000;
    PostionRegs.offset_temp.pos_w = 5000;
    PostionRegs.offset_temp.pos_u = 5000;

    // 初始化Line1,Line2
    /*  Line1.X0 = 0;
        Line1.Y0 = 0;
        Line1.Z0 = 0;
        Line1.X1 = 0;
        Line1.Y1 = 0;
        Line1.Z1 = 0;
        Line1.I  = 0;
        Line1.J  = 0;
        Line1.K  = 0;
        Line1.R  = 0;
        Line1.type = 0;
        Line2.X0 = 0;
        Line2.Y0 = 0;
        Line2.Z0 = 0;
        Line2.X1 = 0;
        Line2.Y1 = 0;
        Line2.Z1 = 0;
        Line2.I  = 0;
        Line2.J  = 0;
        Line2.K  = 0;
        Line2.R  = 0;
        Line2.type = 0;

        mx.velo_x = 1000;           // 输出脉冲最大变化量
        mx.velo_y = 1000;
        mx.velo_z = 1000;
        mx.velo_a = 1000;
        mx.velo_b = 1000;
        mx.velo_c = 1000;

    // 脉冲输出端口清零
    //  EALLOW;
    //  DioRegs.Read_DI0 = 0x0000;                      // X轴脉冲清零
    //  DioRegs.Read_DI1 = 0x0000;                      // Y轴脉冲清零
    //  DioRegs.Read_DI2 = 0x0000;                      // Z轴脉冲清零
    //  DioRegs.Read_DI3 = 0x0000;                      // A轴脉冲清零
    //  DioRegs.Read_DI4 = 0x0000;                      // B轴脉冲清零
    //  DioRegs.Read_DI5 = 0x0000;                      // C轴脉冲清零
    //  DioRegs.LapEn1 = 0x0000;                        // 一转信号不使能
    //  EDIS;
    */
    // 加减速初始化
    //  SpeedRegs.rem_length = 0;           // 段剩余进给长度
    //  EncodeRegs.first1 = 0;
    //  SpeedRegs.sect_Lin.type = -1;       // 上段加工类型
    //  SpeedRegs.sect_Lin.S = 0;           // 上段主轴
    //  SpeedRegs.sect_in.type = -1;    // 本段加工类型
    //  SpeedRegs.sect_Nin.type = -1;     // 下段加工类型
    //  SpeedRegs.updown_stats = 0;     // 加减速状态
    ParamterUnRegs.ParamterRegs.AxisparmRegs.FLimitup = 1000;

    RunstatusRegs.HandwheelFlag = 0;           // 手轮选择
    RunstatusRegs.RunStatusFlag.bit.Pause = 0; // 暂停

    //  RC_bak = PostionRegs.AbsPos;
    MainAxisRegs.S_speed = 0; // 主轴速度
    EncodeRegs.Alarm_Flag = 0;
    SpeedRegs.Epslion = 5;
    SpeedRegs.KL = 5;
    RPDAccDecRegs.KL_e = 5;
    RPDAccDecRegs.RPD_ud_lengthx = 0; // 快速定位加减速长度初始化
    RPDAccDecRegs.RPD_ud_lengthy = 0;
    RPDAccDecRegs.RPD_ud_lengthz = 0;
    RPDAccDecRegs.RPD_ud_lengtha = 0;
    RPDAccDecRegs.RPD_ud_lengthb = 0;
    RPDAccDecRegs.RPD_ud_lengthc = 0;
    RPDAccDecRegs.RPD_ud_lengthw = 0;
    RPDAccDecRegs.RPD_ud_lengthu = 0;

    RunstatusRegs.Alarm.limit.all = 0;      // 限位报警 0xfff->0 change later
    RunstatusRegs.Alarm.servo.all = 0x0000; // 驱动器波阿静
    RunstatusRegs.Alarm.softlimit.all = 0;  // 软限位报警
    RunstatusRegs.Alarm.esp = 0;            // 急停报警  */

    PostionRegs.DuiDao.pos_x = 0;
    PostionRegs.DuiDao.pos_y = 0;
    PostionRegs.DuiDao.pos_z = 0;
    PostionRegs.DuiDao.pos_a = 0;
    PostionRegs.DuiDao.pos_b = 0;
    PostionRegs.DuiDao.pos_c = 0;
    PostionRegs.DuiDao.pos_w = 0;
    PostionRegs.DuiDao.pos_u = 0;

    RunstatusRegs.ReturnFlag.all = 0; // 回参状态清零

    RPDAccDecRegs.ehand_last1 = 0; // 电子手轮上次进给状态
    RPDAccDecRegs.ehand_last2 = 0;
    RPDAccDecRegs.ehand_last3 = 0;
    RPDAccDecRegs.ehand_last4 = 0;
    RPDAccDecRegs.ehand_last5 = 0;
    RPDAccDecRegs.ehand_last6 = 0;
    RPDAccDecRegs.ehand_last7 = 0;
    RPDAccDecRegs.ehand_last8 = 0;
    RPDAccDecRegs.Ei1 = 0; // 后加减速误差累加器
    RPDAccDecRegs.Ei2 = 0;
    RPDAccDecRegs.Ei3 = 0;
    RPDAccDecRegs.Ei4 = 0;
    RPDAccDecRegs.Ei5 = 0;
    RPDAccDecRegs.Ei6 = 0;
    RPDAccDecRegs.Ei7 = 0;
    RPDAccDecRegs.Ei8 = 0;
    RPDAccDecRegs.GM1 = 0; // 后加减速次数
    RPDAccDecRegs.GM2 = 0;
    RPDAccDecRegs.GM3 = 0;
    RPDAccDecRegs.GM4 = 0;
    RPDAccDecRegs.GM5 = 0;
    RPDAccDecRegs.GM6 = 0;
    RPDAccDecRegs.GM7 = 0;
    RPDAccDecRegs.GM8 = 0;
    RPDAccDecRegs.JJFlag1 = 0; // 后加减速状态
    RPDAccDecRegs.JJFlag2 = 0;
    RPDAccDecRegs.JJFlag3 = 0;
    RPDAccDecRegs.JJFlag4 = 0;
    RPDAccDecRegs.JJFlag5 = 0;
    RPDAccDecRegs.JJFlag6 = 0;
    RPDAccDecRegs.JJFlag7 = 0;
    RPDAccDecRegs.JJFlag8 = 0;

    MainAxisRegs.S_BeiLv = 0; // 主轴倍率

    PostionRegs.CurCo.pos_x = 0;
    PostionRegs.CurCo.pos_y = 0;
    PostionRegs.CurCo.pos_z = 0;
    PostionRegs.CurCo.pos_a = 0;
    PostionRegs.CurCo.pos_b = 0;
    PostionRegs.CurCo.pos_c = 0;
    PostionRegs.CurCo.pos_w = 0;
    PostionRegs.CurCo.pos_u = 0;

    PostionRegs.out_imp_bk.pos_a = 0;
    PostionRegs.out_imp_bk.pos_b = 0;
    PostionRegs.out_imp_bk.pos_c = 0;
    PostionRegs.out_imp_bk.pos_x = 0;
    PostionRegs.out_imp_bk.pos_y = 0;
    PostionRegs.out_imp_bk.pos_z = 0;
    PostionRegs.out_imp_bk.pos_w = 0;
    PostionRegs.out_imp_bk.pos_u = 0;

    ParamterUnRegs.ParamterRegs.AxisparmRegs.ExpAcc = 10; // 后加速参数
    ParamterUnRegs.ParamterRegs.AxisparmRegs.SyncCoff1 = 100;
    ParamterUnRegs.ParamterRegs.AxisparmRegs.SyncCoff2 = 100;
    for (i = 0; i < 8; i++)
    {
        ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[i] = 100;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[i] = 400;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[i] = 0.1678; // 后减速参数
        ParamterUnRegs.ParamterRegs.AxisparmRegs.NSoftLimit[i] = -410000000;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.PSoftLimit[i] = 400000000;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[i] = 0.01638;
        ParamterUnRegs.ParamterRegs.AxisparmRegs.J[i] = 0.08;
    }
    EncodeRegs.DaobuFlag = 0;
    ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuFastMoveSpeed = 0.01638;
    ParamterUnRegs.ParamterRegs.AxisparmRegs.ChabuLineAcc = 0.1678;

    //  ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[0]=15;
    //  ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[1]=150;
    //  ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[2]=1500;
    //  ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[3]=1500;
    //  ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[4]=1500;
    //  ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[5]=1500;
    for (i = 0; i < 100; i++)
    {
        ThreeWordsReg.X[i] = 0;
        ThreeWordsReg.Y[i] = 0;
        ThreeWordsReg.Z[i] = 0;
        ThreeWordsReg.A[i] = 0;
        ThreeWordsReg.B[i] = 0;
        ThreeWordsReg.C[i] = 0;
        ThreeWordsReg.W[i] = 0;
        ThreeWordsReg.U[i] = 0;
        ThreeWordsReg.I[i] = 0;
        ThreeWordsReg.J[i] = 0;
        ThreeWordsReg.K[i] = 0;
        ThreeWordsReg.R[i] = 0;
    }
    ThreeWordsReg.ChabuMoveSpeed = 25000;
    ThreeWordsReg.Mode = 0;
    ThreeWordsReg.PosNumX = 0;
    ThreeWordsReg.PosNumY = 0;
    ThreeWordsReg.PosNumZ = 0;
    ThreeWordsReg.PosNumA = 0;
    ThreeWordsReg.PosNumB = 0;
    ThreeWordsReg.PosNumC = 0;
    ThreeWordsReg.PosNumW = 0;
    ThreeWordsReg.PosNumU = 0;
    ThreeWordsReg.ProModeFlag = 90;
    //    ThreeWordsReg.RPGFlag = 0;
    ThreeWordsReg.BGMFlag = 0;
    ThreeWordsReg.ChabuMoveSpeedBeilv = 1.0;
    //    ThreeWordsReg.STMFlag = 0;
    ThreeWordsReg.VICFlag = 17;
    ThreeWordsReg.VACSpeed = 256000;
    ThreeWordsReg.MoveSpeed = 0.01638;
    ThreeWordsReg.MACSpeed = 0.1678;
    ThreeWordsReg.IOLow = 0;
    ThreeWordsReg.IOHigh = 0;
    ThreeWordsReg.iIOLow = 0;
    ThreeWordsReg.iIOHigh = 0;
    ThreeWordsReg.MONFlag = 255;

    // RunstatusRegs.file_download_flag = 0;   // 上位机程序是否下载完成
    // RunstatusRegs.single_download_flag = 0;
    // RunstatusRegs.codeFMHandle.readProgNameAddress = FLASH_PERSIST_PROGNUM_START;
    // RunstatusRegs.codeFMHandle.writeAddress = FLASH_PERSIST_PROGNUM_START;
    // RunstatusRegs.codeFMHandle.isOk = 0;

    // /* flash参数赋值  */
    // for(i = 0; i < 8; i++){
    //     if(bpmParams.HasMspParam[i] == 1){
    //         ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[i] = bpmParams.msp[i];
    //     }

    //     if(bpmParams.HasMacParam[i] == 1){
    //         ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[i] = bpmParams.mac[i];
    //     }
    // }
}
//---------------------------------------------------------------------------
// 函数功能：   更改参数后的初始化
// 输入参数：   无..
// 输出参数：   无
// 创建时间：   2006.06.15
// 创建人  ：   付科
//---------------------------------------------------------------------------
void InitPara_c(void)
{
    EncodeRegs.CsReg.S = ParamterUnRegs.ParamterRegs.MAxisparmRegs.MainDefaultSpeed;      // S代码
    EncodeRegs.AsReg.S = abs(ParamterUnRegs.ParamterRegs.MAxisparmRegs.MainDefaultSpeed); //  abs(*(uint16_t *)0x8006E);          // 0x3D806E);
    // 回参慢速进给速度
    RPDAccDecRegs.UniVelo_R = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevelFL / 15;      //  0x80002B
    if (RPDAccDecRegs.UniVelo_R == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevelFL) // 0x3D802B
        RPDAccDecRegs.UniVelo_R = 1;
    RunstatusRegs.Zero_pol = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefZeroLevel; // *(uint16_t *)0x80025;               // 零点信号电平   0x3D8025
    SpeedRegs.speed_min = ParamterUnRegs.ParamterRegs.AxisparmRegs.FLimitup / 15;      // (*(uint16_t *)0x80027)/15; // 指数加减速低速下限 0x3D8027//陈林
    if ((SpeedRegs.speed_min == 0) && ParamterUnRegs.ParamterRegs.AxisparmRegs.FLimitup)
        SpeedRegs.speed_min = 1;
    RunstatusRegs.ret_offset = ParamterUnRegs.ParamterRegs.MAxisparmRegs.RetRefDistance; //*(uint16_t *)0x80077;       // 偏置距离
    RPDAccDecRegs.spd_up_par = ParamterUnRegs.ParamterRegs.AxisparmRegs.ExpAcc;          //(*(uint16_t *)0x80057) ; 4 指数加减速参数//定值250//cl
    if (RPDAccDecRegs.spd_up_par < 2 && RPDAccDecRegs.spd_up_par)
        RPDAccDecRegs.spd_up_par = 2;
    RPDAccDecRegs.spd_down_par1 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[0] / 4; // 0x80032线性加减速参数x
    if (RPDAccDecRegs.spd_down_par1 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[0])
        RPDAccDecRegs.spd_down_par1 = 2;
    RPDAccDecRegs.spd_down_par2 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[1] / 4; // 0x80033线性加减速参数y
    if (RPDAccDecRegs.spd_down_par2 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[1])
        RPDAccDecRegs.spd_down_par2 = 2;
    RPDAccDecRegs.spd_down_par3 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[2] / 4; // 0x80034线性加减速参数z
    if (RPDAccDecRegs.spd_down_par3 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[2])
        RPDAccDecRegs.spd_down_par3 = 2;
    RPDAccDecRegs.spd_down_par4 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[3] / 4; // 0x80035线性加减速参数a
    if (RPDAccDecRegs.spd_down_par4 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[3])
        RPDAccDecRegs.spd_down_par4 = 2;
    RPDAccDecRegs.spd_down_par5 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[4] / 4; // 0x80036线性加减速参数b
    if (RPDAccDecRegs.spd_down_par5 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[4])
        RPDAccDecRegs.spd_down_par5 = 2;
    RPDAccDecRegs.spd_down_par6 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[5] / 4; // 0x80037线性加减速参数c
    if (RPDAccDecRegs.spd_down_par6 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[5])
        RPDAccDecRegs.spd_down_par6 = 2;
    RPDAccDecRegs.spd_down_par7 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[6] / 4; // 0x80038线性加减速参数w
    if (RPDAccDecRegs.spd_down_par7 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[6])
        RPDAccDecRegs.spd_down_par7 = 2;
    RPDAccDecRegs.spd_down_par8 = ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[7] / 4; // 0x80039线性加减速参数c
    if (RPDAccDecRegs.spd_down_par8 < 2 && ParamterUnRegs.ParamterRegs.AxisparmRegs.LineAcc[7])
        RPDAccDecRegs.spd_down_par8 = 2;
    PostionRegs.out_add.pos_x = 0;
    PostionRegs.out_add.pos_y = 0;
    PostionRegs.out_add.pos_z = 0;
    PostionRegs.out_add.pos_a = 0;
    PostionRegs.out_add.pos_b = 0;
    PostionRegs.out_add.pos_c = 0;
    PostionRegs.out_add.pos_w = 0;
    PostionRegs.out_add.pos_u = 0;
    MainAxisRegs.PSANGN = ParamterUnRegs.ParamterRegs.MAxisparmRegs.AnaOutput; // *(uint16_t *)0x80076主轴模拟输出调整数据
    PostionRegs.CurCo.pos_x = ParamterUnRegs.ParamterRegs.G54.X;
    PostionRegs.CurCo.pos_y = ParamterUnRegs.ParamterRegs.G54.Y;
    PostionRegs.CurCo.pos_z = ParamterUnRegs.ParamterRegs.G54.Z;
    PostionRegs.CurCo.pos_a = ParamterUnRegs.ParamterRegs.G54.A;
    PostionRegs.CurCo.pos_b = ParamterUnRegs.ParamterRegs.G54.B;
    PostionRegs.CurCo.pos_c = ParamterUnRegs.ParamterRegs.G54.C;
    PostionRegs.CurCo.pos_w = ParamterUnRegs.ParamterRegs.G54.W;
    PostionRegs.CurCo.pos_u = ParamterUnRegs.ParamterRegs.G54.U;
}

// 清除AsReg, BsReg, CsReg寄存器及运行状态标志
// 限位开关有效后，清除当前工作状态
void CLEARABC()
{
    memset(&EncodeRegs.CsReg, 0, sizeof(struct UnReg));
    EncodeRegs.CsReg.GX1 = GX1DEFAULT; // 基代码分�
    EncodeRegs.CsReg.GX2 = GX2DEFAULT; // 默认G00
    EncodeRegs.CsReg.GX3 = GX3DEFAULT;
    EncodeRegs.CsReg.GX4 = GX4DEFAULT;
    EncodeRegs.CsReg.GX5 = GX5DEFAULT;
    EncodeRegs.CsReg.GX6 = GX6DEFAULT;
    EncodeRegs.CsReg.GX7 = GX7DEFAULT;
    EncodeRegs.CsReg.GX8 = GX8DEFAULT;
    EncodeRegs.CsReg.I = 0;    // 圆弧插补 X轴圆心-起点
    EncodeRegs.CsReg.J = 0;    // 圆弧插补 Y轴圆心-起点
    EncodeRegs.CsReg.K = 0;    // 圆弧插补 Z轴圆心-起点
    EncodeRegs.CsReg.F = 0;    // 进给速度
    EncodeRegs.CsReg.M = 0;    // M代码
    EncodeRegs.CsReg.M1 = 100; // M代码
    EncodeRegs.CsReg.M2 = 5;
    EncodeRegs.CsReg.M3 = 100;
    EncodeRegs.CsReg.M4 = 0; // M08 开冷却液      M09关冷却液
    EncodeRegs.CsReg.M5 = 0; // M32 润滑开       M33润滑关
    EncodeRegs.CsReg.M6 = 0;
    EncodeRegs.CsReg.M7 = 0;
    EncodeRegs.CsReg.M8 = 0;
    EncodeRegs.CsReg.M9 = 0;
    EncodeRegs.CsReg.M10 = 0;
    EncodeRegs.CsReg.T = 0; // T代码
    EncodeRegs.CsReg.R = 0; // 圆弧半径
    EncodeRegs.CsReg.P = 0; // G04等待时间

    EncodeRegs.BsReg = EncodeRegs.CsReg;

    EncodeRegs.AsReg.Type = WTYPEDEFAULT; // 工作方式标志
    EncodeRegs.AsReg.I = 0;               // 圆弧插补 圆心-起点
    EncodeRegs.AsReg.J = 0;
    EncodeRegs.AsReg.K = 0;
    EncodeRegs.AsReg.P = 0; // G04等待时间
    EncodeRegs.AsReg.F = 0; // 进给速度
    EncodeRegs.AsReg.M = 0; // M代码
    EncodeRegs.AsReg.M1 = 100;
    EncodeRegs.AsReg.M2 = 100;
    EncodeRegs.AsReg.M3 = 100;
    EncodeRegs.AsReg.Co = 54;   // 坐标系
    EncodeRegs.AsReg.Abs = 0;   //
    EncodeRegs.AsReg.Plate = 0; // 平面选择

    RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
    EncodeRegs.IncodeFlag = 0;
    RunstatusRegs.RpdFlag = 0;
    RunstatusRegs.RunStatusFlag.bit.AutoJud = 0;
    PostionRegs.real_posbk = PostionRegs.real_pos;

    //  jointsBK = joints;  //glh
    RunstatusRegs.G28Flag = 0;
    RunstatusRegs.ReturnFlag.all = 0;

    // 加减速初始化
    //  EncodeRegs.first1 = 0;
    SpeedRegs.sect_Lin.type = -1; // 加工类型
    SpeedRegs.sect_Lin.S = 0;
    SpeedRegs.sect_in.type = -1;  // 加工类型
    SpeedRegs.sect_Nin.type = -1; // 加工类型
    CirCleInterRegs.st_end_cir.x_st = 0;
    CirCleInterRegs.st_end_cir.y_st = 0;
    CirCleInterRegs.st_end_cir.x_end = 0;
    CirCleInterRegs.st_end_cir.y_end = 0;
    CirCleInterRegs.st_end_cir.cen_x = 0;
    CirCleInterRegs.st_end_cir.cen_y = 0;
    CirCleInterRegs.st_end_cir.cir_r = 0;
    CirCleInterRegs.st_end_cir.velo = 0;
    RunstatusRegs.ProNum = 0;
    EncodeRegs.DaobuFlag = 0;
}
