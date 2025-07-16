#ifndef __VARIABLEDEF_H__
#define __VARIABLEDEF_H__

#include  <stdint.h>

#ifdef __cplusplus
extern "C" {

#endif

// @author zxc
// @brief define constants
// @revision 1.0 by zxc on 2025-07-11
#define SLAVE_NUM 7
// 定义每个轴的两个 bit 的索引位置（P/R）
enum {
    LIMIT_XP = 0,
    LIMIT_XR = 1,
    LIMIT_YP = 2,
    LIMIT_YR = 3,
    LIMIT_ZP = 4,
    LIMIT_ZR = 5,
    LIMIT_AP = 6,
    LIMIT_AR = 7,
    LIMIT_BP = 8,
    LIMIT_BR = 9,
    LIMIT_CP = 10,
    LIMIT_CR = 11,
    LIMIT_WP = 12,
    LIMIT_WR = 13,
    LIMIT_UP = 14,
    LIMIT_UR = 15
};


enum STATUS
{idle,BUSY,END};

//typedef int int8_t;
//typedef uint16_t int8_t;
//typedef char int8_t;
#define pi  3.1415926
#define CODESEGEND ';'
#define DOWNCMDEND '&'
#define SEGMAXCHAR  256

extern int countTime;
extern struct CToMDataStruct CToMRegs;

#define SAVECOORDNUM  3 // 保存几组坐标
#define CNCCOORDStaaddr  0     // 坐标起始地址
#define CNCCOORDLength  480    // 坐标数据长度 4 * 12 * 10
#define CNCPARMStaaddr  512    // 参数起始地址
#define CNCPARMLength   350    // 参数数据长度
#define CNCCODEStaaddr  512+4096  //G代码起始地址
#define CNCCODELength   80 * 4  //G代码长度




/* 需要保存的 BPM 参数 */
typedef struct BPMParams {
    uint16_t controllerVersion;

    /* MAC 点位运动加速度参数 */
    float mac[8] ;
    uint16_t HasMacParam[8];
    /* MSP 点位运动速度参数 */
    float msp[8] ;
    uint16_t HasMspParam[8];

    char  tsv[10];
    char  tfv[10];

    uint16_t  IPA[4];


    /* EGM */
    /* DLZ */
    /* TSV */
    /* EGG */
    /* MOF */
    /* DSP */
    /* MDC */
    /* EGR */
    /* DMT */
    /* IPA */
    /* MOE */
    /* IAC */
    /* IDC */
    /* SFL */
    /* EO */
    /* MIT */
    /* BOP */
    /* ERS */
    /* FMD */
    /* ISP */
    /* PL */
    /* IT */
    /* DCE */
    /* SRL */
    /* SET */
    /* MPS */
    /* ISP */
} BPMParams;

struct POINTDATA{
    long pArrayPulse[6];
    long pArrayXYZ[6];
    int16_t r;
    int16_t s;
    int16_t t;
};
struct  PUSHPOP
{
    uint16_t *Add;        //备份当前NC代码的地址
    int32_t ProNum;       //备份当前的光标号
    int   Flag;         //当前的地址是否有效
    int   SubName;      //子程序NC代码名称
    int   MainName;     //主程序NC代码名称
    int   G9X;          //G98,G99
};
struct LIMIT_BITS   {
    uint16_t xp:1;
    uint16_t xr:1;
    uint16_t yp:1;
    uint16_t yr:1;
    uint16_t zp:1;
    uint16_t zr:1;
    uint16_t ap:1;
    uint16_t ar:1;
    uint16_t bp:1;
    uint16_t br:1;
    uint16_t cp:1;
    uint16_t cr:1;

    uint16_t wp:1;
    uint16_t wr:1;
    uint16_t up:1;
    uint16_t ur:1;
    //uint16_t rsvd:4;
};
union LIMIT_REG {
    uint16_t all;
    struct LIMIT_BITS bit;
};
struct SERVO_BITS   {
    uint16_t  x:1;
    uint16_t  y:1;
    uint16_t  z:1;
    uint16_t  a:1;
    uint16_t  b:1;
    uint16_t  c:1;
    uint16_t  w:1;
    uint16_t  u:1;
    uint16_t  rvsd:8;
};
union SERVO_REG {
    uint16_t all;
    struct SERVO_BITS bit;
};
struct ALARM_REGS   {
   union SERVO_REG servo;
   union LIMIT_REG limit;
   union LIMIT_REG softlimit;
   uint16_t esp:1;
};
struct inter_in                 //直线插补输入
{
     int32_t x_st;
     int32_t y_st;
     int32_t z_st;
     int32_t a_st;
     int32_t b_st;
     int32_t c_st;
     int32_t w_st;
     int32_t u_st;

     int32_t x_end;
     int32_t y_end;
     int32_t z_end;
     int32_t a_end;
     int32_t b_end;
     int32_t c_end;
     int32_t w_end;
     int32_t u_end;

     int32_t velo;
};
struct inter_step                //各轴步长
{
     int32_t velo_x;
     int32_t velo_y;
     int32_t velo_z;
     int32_t velo_a;
     int32_t velo_b;
     int32_t velo_c;
     int32_t velo_w;
     int32_t velo_u;
};
struct inter_pos                //各轴当前位置
{
     int32_t pos_x;
     int32_t pos_y;
     int32_t pos_z;
     int32_t pos_a;
     int32_t pos_b;
     int32_t pos_c;
     int32_t pos_w;
     int32_t pos_u;

     int32_t pos_D;
     int32_t pos_E;
     int32_t pos_N;
     int32_t pos_O;
     int32_t pos_Q;
     int32_t pos_V;
     int32_t pos_N7;
     int32_t pos_N8;
     ////////////////////////////
//   int32_t Alfa;
//   int32_t Gama;
//   int32_t Beta;
};
struct inter_in_cir                 //圆弧插补输入
{
     int32_t x_st;
     int32_t y_st;
     int32_t x_end;
     int32_t y_end;
     int32_t cen_x;
     int32_t cen_y;
     int32_t cir_r;
     int32_t velo;
};
//定义G52-G59的代码结构
struct PosClass
{
    int32_t   X;
    int32_t   Y;
    int32_t   Z;
    int32_t   A;
    int32_t   B;
    int32_t   C;
    int32_t   W;
    int32_t   U;
};// 回参使用
struct axis_status                  // 各轴状态寄存器
{
    int x;
    int y;
    int z;
    int a;
    int b;
    int c;
    int w;
    int u;
};
// 加减速通用寄存器
struct SpeedSTRRegs
{
    int type;               // 加工类型
    uint16_t S;                       // S代码
    struct inter_pos st;
    struct inter_pos end;
    int32_t i;                // 圆弧半径参数
    int32_t j;
    int32_t k;
    int32_t R;
    int32_t plane;
    int32_t length;               // 程序轨迹长度
    int32_t feed;                 // 程序编程速度
    int32_t fc;                   // 拐点临界速度
    int32_t vi;                   // 当前步(i步)速度
    int flag;               // 加减速处理标志
};
struct FLAG_BITS    {
    uint16_t x:1;
    uint16_t y:1;
    uint16_t z:1;
    uint16_t a:1;
    uint16_t b:1;
    uint16_t c:1;
    uint16_t w:1;
    uint16_t u:1;
    uint16_t m:1;
    uint16_t n:1;
    uint16_t k:1;
    uint16_t o:1;
    uint16_t p:1;
    uint16_t q:1;
    uint16_t rsvd:2;

};
union FLAG_REG  {
    uint16_t all;
    struct FLAG_BITS bit;
};
struct RCCompStruct           // 反向间隙
{
    int16_t  RC_x;                  // 反向间隙各轴状态标志
    int16_t  RC_y;
    int16_t  RC_z;
    int16_t  RC_a;
    int16_t  RC_b;
    int16_t  RC_c;
    int16_t  RC_w;
    int16_t  RC_u;
    uint16_t RC_x_length;       // 反向间隙长度
    uint16_t RC_y_length;       // 反向间隙长度
    uint16_t RC_z_length;       // 反向间隙长度
    uint16_t RC_a_length;       // 反向间隙长度
    uint16_t RC_b_length;       // 反向间隙长度
    uint16_t RC_c_length;       // 反向间隙长度
    uint16_t RC_w_length;       // 反向间隙长度
    uint16_t RC_u_length;       // 反向间隙长度
    struct inter_pos RC_bak;                // 上次进给状态
};
struct CommParmStruct           // 通用参数
{
    uint16_t  EXTIO;                // I/O扩展;1: 有I/O扩展    0: 无I/O扩展
    uint16_t  EleHandWheel;         // 电子手轮:  1: 手轮方式 0：点动方式
    uint16_t  ConterClr;            // 记数单元清零: 零件记数单元清零标志 1：单元清零  0：零件记数单元开机保持原值
    uint16_t  BackToolDir;          // 退刀方向:     0x00  0x01  0x10  0x11       +X   -X    +Y    -Y
    uint16_t  CanTimes;             // 去抖动时间
    uint16_t  OrderInc;             // 顺序号增量
    uint16_t  G73BackToolV;         // G73退刀量
    uint16_t  G83StartP;            // G83切削始点
    uint16_t  MTTimes;              // M处理时间
    uint16_t  STTimes;              // S处理时间
    uint16_t  BeepTimes;            // 蜂鸣器时间
    uint16_t  SelRotateAxis;        // 旋转轴选择
    uint32_t  InputBaud;            // 输入波特率
    uint32_t  OutputBaud;           // 输出波特率
    int32_t   PCZeroPoint[8];       // 螺补零点
};
struct AxisParmStruct
{
    uint16_t  GridSignal;     // 栅格信号      //z脉冲2023
//    uint16_t  FeSwitch;       // 磁开关 1: X 轴有机械零点。（回零方式B）  0: X轴无机械零点。（回零方式A）
    uint16_t  AlarmLevel;     // 报警信号电平0：高电平  1：低电平
    uint16_t  ReadyLevel;     // 准备信号电平0：高电平  1：低电平
    uint16_t  MotorRotDir;    // 电机旋转方向 0：正传  1：反转
    uint16_t  ToolNoSignal;   // 刀位信号 0：刀位信号高电平有效 1：刀位信号低电平有效
    uint16_t  ToolLockSgn;    // 刀架锁紧信号0：刀架锁紧信号高电平有效  1：刀架锁紧信号低电平有效
    uint16_t  ToolNoTest;     // 刀位号检测0：刀位号直接检测   1：刀位号编码检测
    uint16_t  RetRefLevel;    // 回参减速信号电平 0：高电平   1：低电平
    uint16_t  RetRefZeroLevel;  // 回参零点信号电平 0：高电平 1：低电平
    uint16_t  FLimitup;            // 切削进给上限
//    uint16_t  FLimitdn;        // 切削FL下限
//    uint16_t  ManuFLimitup;    // 手动进给FL
//    uint16_t  FSSPeed;        // 切削进给初速
//    uint16_t  MoveRatios;      // 快移倍率F0
    uint16_t  RetRefLevelFL;    // 回参考点FL
    float  FastMoveSpeed[8];   // 快移速度
    float  LineAcc[8];         // 快移线性加速
    float  ChabuFastMoveSpeed;
    float  ChabuLineAcc;
    float  RCCompValue[8];     // 间隙补偿 6 -> 8
    uint32_t  RetRefCoord[8];     // 回参坐标
    uint32_t  PCInt[8];           // 螺补间隙 6 -> 8
    uint16_t  PulseStart;         // 脉冲控制启停 0:停止  1:启动
    uint16_t  ExpAcc;             // 指数加减速
    uint16_t  MulRatios[8];       // 指令倍乘比
    uint16_t  DivideRatios[8];    // 指令分频
//    uint16_t  WheelRatios;        // 齿轮比
    uint16_t  SyncAxisSel;        // 同步轴选择 001 :XY     010: XZ 011: YZ 100: XA 101: YA  110: ZA
    uint16_t  SyncCoff1;        // 同步系数1
    uint16_t  SyncCoff2;        // 同步系数2
    long  NSoftLimit[8];     // 负向行程极限
    long  PSoftLimit[8];       // 正向行程极限
    uint32_t  ZeroShift[8];
    float   ZeroSpeed[8];
    float   J[8];
//    uint16_t  MotorDir[8];
    uint16_t  ZeroDir[8];
};
struct MainAxisParmStruct
{
    uint16_t  StopSignal;     // 制动信号0：主轴停止时不输出制动信号   1：主轴停止时输出制动信号
    uint16_t  SwitchOut;      // 档位输出0：主轴档位直接输出   1：主轴档位编码输出
    uint16_t  S1RotSpeed;        // 1档转速
    uint16_t  S2RotSpeed;     // 2档转速
    uint16_t  S3RotSpeed; // 3档转速
    uint16_t  S4RotSpeed; // 4档转速
    uint16_t  MainDefaultSpeed;    // 主轴默认初速
    uint16_t  SelS2s4;     // S2/4选择 0:S2 1:S4
    uint16_t  SSControlMode;    // 启停控制方式0：主轴启停控制为电平方式    1：主轴启停控制为脉冲方式
    uint16_t  OffsetComp;    // 偏置补偿值
    uint16_t  StopDelay;      // 停止信号延迟
    uint16_t  SSSgnDelay;    // 制动信号延迟
    uint16_t  SSOutputTime;    // 制动输出时间
    uint16_t  AnaOutput;      // 模拟输出调整
    uint16_t  RetRefDistance;    // 回参距离
    uint32_t  NSoftLimit[8];     // 负向行程极限
    uint32_t  PSoftLimit[8];       // 正向行程极限
    uint16_t  ServoAlarmLev;       // 伺服报警电平
    uint16_t  MachZero;       // 机械零点
    uint16_t  LimitLevel  ;            // 限位开关电平
    uint16_t  Resv[13];         // 备份参数
};
struct ParamterStruct
{
    struct CommParmStruct m_commonparmRegs;
    struct AxisParmStruct AxisparmRegs;
    struct MainAxisParmStruct MAxisparmRegs;
    struct PosClass G52,G53,G54,G55,G56,G57,G58,G59;
    int16_t  PCCommpX[128];   // 128
    int16_t  PCCommpY[128];   // 128
    int16_t  PCCommpZ[128];   // 128
    int16_t  PCCommpA[128];   // 128
    int16_t  PCCommpB[128];   // 128
    int16_t  PCCommpC[128];   // 128

    int32_t  xh[12];

};
union ParamUnion
{
    struct ParamterStruct ParamterRegs;
    char  RecvBuff[1040];
};
extern uint16_t CNCCodeChar[CNCCODELength];  // 12*
extern union ParamUnion ParamterUnRegs;
//定义通用寄存器
//CsReg/BsReg
struct UnReg
{
    int32_t   N;
    int8_t    GX1;        //G代码组1 主要是固定循环
    int8_t    GX2;        //G代码组2 运动控制
    int8_t    GX3;        //G代码组3 刀补代码
    int8_t    GX4;        //G代码组4 坐标系
    int8_t    GX5;        //G代码组5 绝对相对坐标
    int8_t    GX6;        //G代码组6 G20 G21
    int8_t    GX7;        //G代码组7 坐标平面17 18 19
    int8_t    GX8;        //G代码组8 G11 G12
    int8_t     H;         //刀补号
    int32_t   X;          //X轴坐标值
    int32_t   Y;          //Y轴坐标值
    int32_t   Z;          //Z轴坐标值
    int32_t   A;          //A轴坐标值
    int32_t   B;          //B轴坐标值
    int32_t   C;          //C轴坐标值
    int32_t   W;          //B轴坐标值
    int32_t   U;          //C轴坐标值

    int32_t   I;          //圆心坐标-起点X轴坐标
    int32_t   J;          //圆心坐标-起点Y轴坐标
    int32_t   K;          //圆心坐标-起点Z轴坐标
    int32_t   R;          //圆半径
    int32_t   F;          //进给速度
    int32_t    FZ;        //螺旋线插补垂直轴速度
    int32_t   S;          //S代码
    int32_t   T;          //T代码
    int8_t    M;          //M代码
    int8_t    M1;     //M00 程序停机,     M01 选择停机
    int16_t    M2;        //M03 主轴顺转启动  M04 主轴逆转启动
    int8_t    M3;     //M98 子程序调用    M99子程序结束
    int16_t    M4;        //M08 开冷却液      M09关冷却液
    int8_t     M5;        //M32 润滑开       M33润滑关
    int8_t    M6;
    int8_t    M7;        //M43 低氧          M44断低氧
    int8_t    M8;        //M45 高氧          M46断高氧
    int8_t    M9;        //M47 乙炔          M48断乙炔
    int8_t    M10;       //M49割枪伸出       M50割枪缩回
    int32_t   P;          //G04 的暂停时间
    int32_t   Flag;       //标志位
    int32_t    length;    //段长度
    int32_t   R_cir;     // 09.1.3
    int8_t    DaoFlag;   //09.1.6刀补固定循环计算长度计算FLAG
    int8_t    DanbuFlag;  // 单步标志 处理固定循环时单步问题
    int8_t    L;          //拐点到位级别
    uint16_t  Value;      //定时时间
    int32_t Alfa;
    int32_t Gama;
    int32_t Beta;
//glh
    uint8_t M11;           //输入输出
    uint8_t M12;           // 设定电压 电流值
    uint8_t M13;           // 平移、旋转
//  uint8_t MCode[16];
    int8_t o;             //字节型
    int16_t i;            //整数型
    int32_t l;            //长整数型
    int32_t f;          //浮点型
    int8_t t;            //输入信号
    int8_t g;
    int8_t h;
    int8_t n;
    int8_t k;
    int8_t j;           //输出信号
    int8_t c;
    int8_t d;
    int8_t e;           //时间
    int32_t   D;          //关节1角度
    int32_t   E;          //关节2角度
    int32_t   N1;         //关节3角度
    int32_t   O;          //关节4角度
    int32_t   Q;          //关节5角度
    int32_t   V;          //关节6角度
    int32_t   N7;          //关节7角度
    int32_t   N8;          //关节8角度
    int8_t    HaveMCode;
    int8_t    HaveTCode;
    float  MoveSpeed;  //快速移动速度
    float  MoveAccSpeed; //快速移动加速度
    float  ChabuAccSpeed;
    uint16_t IOLow;
    uint16_t IOHigh;
    uint16_t MotorEnable;
};
//AsReg
struct RunReg{
    int8_t    Type;       // //0,1----直线，2－－－顺圆，3－－－逆圆(G00\G01\G02\G03)
    int32_t   X;          // 各轴坐标
    int32_t   Y;
    int32_t   Z;
    int32_t   A;
    int32_t   B;
    int32_t   C;
    int32_t   W;
    int32_t   U;
    int32_t   F;          // 进给速度
    int32_t   FW;     //螺旋线插补圆弧速度
    int32_t   FZ;     //螺旋线插补垂直轴速度
    int32_t   S;          // S代码
    int32_t   I;          // 圆心坐标-起点X轴坐标
    int32_t   J;          // 圆心坐标-起点Y轴坐标
    int32_t   K;          // 圆心坐标-起点Z轴坐标
    int32_t   P;          // G04暂停时间
    int8_t    M;          // M代码
    int8_t    M1;         //M00 程序停机,     M01 选择停机
    int8_t    M2;         //M03 主轴顺转启动  M04 主轴逆转启动    //M05主轴停止
    int8_t    M3;         //M98 子程序调用    M99子程序结束
    int8_t    M4;         //M08 开冷却液      M09关冷却液
    int8_t    M5;         //M32 润滑开       M33润滑关
    int8_t    M6;
    int8_t    M7;        //M43 低氧          M44断低氧
    int8_t    M8;        //M45 高氧          M46断高氧
    int8_t    M9;        //M47 乙炔          M48断乙炔
    int8_t    M10;       //M49割枪伸出       M50割枪缩回
    int32_t   Co;     // 坐标系
    int32_t   Plate;  // 坐标平面
    int8_t    Abs;        // 绝对相对坐标
    int8_t    GX1;        // G代码组1,运动控制
    int32_t   length;    //段长度// 09 04 07
    int32_t   R_cir;     // 09.1.3
    int32_t   Flag;       //标志位
    int8_t    DaoFlag;   //09.1.6刀补固定循环计算长度计算FLAG
    int8_t    DanbuFlag;  //单步标志 处理固定循环时单步问题
    int8_t    L;          //拐点到位级别
    int32_t   Alfa;
    int32_t   Gama;
    int32_t   Beta;
    int32_t   D;          //关节1角度
    int32_t   E;          //关节2角度
    int32_t   N;          //关节3角度
    int32_t   O;          //关节4角度
    int32_t   Q;          //关节5角度
    int32_t   V;          //关节6角度
    int32_t   N7;          //关节7角度
    int32_t   N8;          //关节8角度
    int8_t    MCode[16];
    int8_t    HaveMCode;
    int8_t    HaveTCode;
    uint16_t IOLow;
    uint16_t IOHigh;
    uint16_t MotorEnable;
};

//刀具C功能补偿时 线段特征结构体
struct S_Line
{
    int   type; //0,1----直线，2－－－顺圆，3－－－逆圆(G00\G01\G02\G03)
    int32_t X0;   //线段起点
    int32_t Y0;
    int32_t Z0;
    int32_t X1;   //线段终点
    int32_t Y1;
    int32_t Z1;
    int32_t I;    //圆弧时 的 I
    int32_t J;    // J
    int32_t K;
    int32_t R;
};
//刀具C功能补偿时 线段斜率结构体
struct S_Slope
{
    int   G;     //何种平面下的刀补 0－－－G17，1－－G18，2－－G19(GX7)
    int32_t Xl;
    int32_t Yl;
    int32_t Zl;
    int32_t length;
};
//////返回值结构体
struct RTNDATA
{
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t f;
};
struct  INT64
{
    uint32_t int_l;
    int32_t  int_h;
};
struct length_stepNum                 //直线插补输入
{
     int32_t length;
     int32_t RemLen;
     int32_t stepNum;
};
struct MainAxisST
{
    uint16_t S_BeiLv;                 // 主轴倍率
    int32_t BeiLv;                    // 倍率
    struct axis_status dir_comp;            // 方向调整完成标志
    struct axis_status low_speed;       // 低速状态标志
    struct axis_status slow_bak;            // 上次减速信号
    int S_speed;                    // 主轴速度
    uint16_t S_button ;               // 主轴按钮状态
    volatile int PWM_out;           // PWM占空比50%
    int S_time;                     // S代码时间
    uint16_t ServoXAlarmCounter;              // 驱动器报警计数器
    uint16_t ServoYAlarmCounter;
    uint16_t ServoZAlarmCounter;
    uint16_t ServoAAlarmCounter;
    uint16_t ServoBAlarmCounter;
    uint16_t ServoCAlarmCounter;
    uint16_t ServoWAlarmCounter;
    uint16_t ServoUAlarmCounter;
    uint16_t PSANGN;                  // 主轴模拟输出调整数据

};
extern struct MainAxisST MainAxisRegs;
struct DbSTREGS
{
    int DbJud;                  // 刀补建立/进行/撤销
    int PlJud;                  // 平面判断
    int k,n,BkNum;              // 刀补后对运算出来的字符串进行计算并翻译
    int32_t D;                            // 刀补半径
    int G4XJud;                 // 判断左右刀补
    int DbNum,DBNum;            // 判断刀补建立/进行(使用的控制)
    uint8_t DBFlag;
    uint32_t DB_R;            //刀具半径的开始地址
    uint16_t *DB_RAdd;
    struct RTNDATA DB_Start_Point;
    struct S_Line Line1,Line2;          //计算刀补前后的线段
    char  DbOutChar[50];            // 定义前后两段程序参数的结构体
};
extern struct DbSTREGS DBRegs;
struct AccDecVarStruct
{
    volatile int32_t ud_length1;             // 理论段首加减速区间长度
    volatile int32_t ud_length2;
    volatile int32_t O_feed1 ;                    // 段首目标速度
    volatile int32_t O_feed2 ;                    // 段尾目标速度
    volatile int32_t ud_stepNum1 ;               // 理论段首加减速区间步数
    volatile int32_t ud_stepNum2 ;           // 理论段尾加减速区间步数
    int32_t   adjust_feed_flag;                      // 匀速段速度调整标志，=1时需调整
    int32_t   adjust_feed,adjust_feed_BK;                        //调整后的指令速度 零点
    int32_t   delta_feed_cnt;                    //
    int32_t   sect_in_feed_BK,sect_in_feed_BK1;    //chenlin2014.3.14
    int32_t   length;       // shengyuduanchang
    int32_t   remLen;
    int32_t   rem_length;
    int32_t   Sect_in_feed;//0601
    int32_t   sect_in_fc;
    int32_t   count_w1;
    int32_t   Tm,ax,ay,az,aa,ab,ac,aw,au;
    int16_t   speedini_count;
    struct  inter_pos Ac;
//    int32_t   JJSpeed;
    float   JJSpeed;

    int32_t   Amax;                  // 最大合成加速度
    int32_t   tm;                    // 至最大加速度时间
    int32_t   Nmax,iNmax;                    // 至最大加速度插补周期数
    int32_t   maxEr;                 // 迭代终止相差范围
    int32_t   speed_min;          // 减速最小速度
    int8_t    updown_stats;       // 加减速状态
    struct SpeedSTRRegs sect_Lin, sect_in, sect_Nin;
    int32_t  spd_down_step,spd_down_step2,spd_down_step3;
    int32_t  spd_up_step,spd_up_step2,spd_up_step3;
    int32_t  a_ini_up,a_ini_down;
    int32_t   Epslion;
    int32_t   KL;

};
struct JOGSTREGS
{
    struct inter_pos Jvi;       // 加减速中间变量
    uint8_t JogSel;
    int Jog_dir;                 // Jog
};
extern struct JOGSTREGS JOGRegs;
struct EncodeSTReg
{
    struct RunReg AsReg;                    // AS的结构
    struct UnReg BsReg;                 // BS的结构
    struct UnReg CsReg;                 // CS的结构
    struct UnReg CsRegBK;               // 备份上一次的CS的值
    struct UnReg CsNext;                    // 下一个CS的值
    uint8_t ThreeWord2CsFlag;
    uint8_t Cs2BsFlag;                //译码流程中的标志
    uint8_t Bs2AsFlag;                //译码流程中的标志
    uint8_t AsFlag;                   //译码流程中的标志
    uint8_t DaobuFlag;
    uint8_t IncodeFlag;                   // 内部代码缓冲区种有新数据标志
    int   PTime ;                       //G04等待时间备份
    uint16_t *subRetAdd;      //子程序返回地址
    uint16_t *IncodeAdd;                      // 设置内部代码缓冲区的地址
    uint16_t *IncodeAddBK;                    // 备份内部代码缓冲区的地址
    uint16_t  bufSubPro[32];
    uint16_t *IncodeBkOfWhile;
    uint16_t Paralength;
    int16_t Alarm_Flag;                   // 报警标志
    uint8_t st_pos_Flag;   //起始坐标标志
    struct SpeedSTRRegs  sect_Temp;      //段长计算临时结构体
//    int16_t first1;
    unsigned char EncodeBuff[SEGMAXCHAR];
};
extern struct EncodeSTReg EncodeRegs;
struct LineInterST
{
    int32_t lin_remx;
    int32_t lin_remy;
    int32_t lin_remz;
    int32_t lin_rema;
    int32_t lin_remb;
    int32_t lin_remc;
    int32_t lin_remw;
    int32_t lin_remu;
    int32_t add_x,add_y,add_z,add_a,add_b,add_c,add_w,add_u;// 直线余数处理累加器
    int32_t length;                       // 起点到终点距离
    int32_t velo_add;                     // 步长累加器
    struct inter_step velo_step;            // 各轴步长
    int32_t  finished_inter_pose;         //标记姿态插补可以完成标志
    int32_t finished_length;//关节插补时已经走过的长度，用于测试2014.6.21
//  struct S_Line Line1,Line2;          //计算刀补前后的线段
};
struct RPDAccDECST
{
    int32_t RPD_ud_lengthx, RPD_ud_lengthy, RPD_ud_lengthz,RPD_ud_lengtha, RPD_ud_lengthb, RPD_ud_lengthc, RPD_ud_lengthw, RPD_ud_lengthu;
    int32_t spd_down_velo1, spd_down_velo2, spd_down_velo3,spd_down_velo4, spd_down_velo5, spd_down_velo6, spd_down_velo7, spd_down_velo8;
    uint16_t RPD_vx, RPD_vy, RPD_vz, RPD_va, RPD_vb, RPD_vc, RPD_vw, RPD_vu;
    int32_t Ei1, Ei2, Ei3, Ei4, Ei5, Ei6, Ei7, Ei8;
    int   GM1, GM2, GM3, GM4, GM5, GM6, GM7, GM8;
    int   JJFlag1, JJFlag2, JJFlag3, JJFlag4, JJFlag5, JJFlag6, JJFlag7, JJFlag8;
    int32_t   KL_e;
    int32_t  ehand_last1,ehand_last2,ehand_last3,ehand_last4,ehand_last5,ehand_last6,ehand_last7,ehand_last8;
    int32_t  UniVelo_X,UniVelo_Y,UniVelo_Z,UniVelo_A,UniVelo_B,UniVelo_C,UniVelo_W,UniVelo_U;// 6轴快速进给速度
    int32_t  UniVelo_R;                       // 慢速回参进给
    uint16_t spd_up_par;      // 指数加减速参数
    uint16_t spd_down_par1;   // 线性加减速参数x
    uint16_t spd_down_par2;   // 线性加减速参数y
    uint16_t spd_down_par3;   // 线性加减速参数z
    uint16_t spd_down_par4;   // 线性加减速参数a
    uint16_t spd_down_par5;   // 线性加减速参数b
    uint16_t spd_down_par6;   // 线性加减速参数c
    uint16_t spd_down_par7;   // 线性加减速参数W
    uint16_t spd_down_par8;   // 线性加减速参数U
//  int32_t  speed_min;           // 减速最小速度

};
extern struct RPDAccDECST RPDAccDecRegs;
struct RunStatusFlagST
{
    uint16_t CBAFlag:1;                       // 0流程控制
    uint16_t Pause: 1;                        // 1暂停
    uint16_t Type:4;                          // 2,3,4,5 工作模式定义/0,1----直线，2－－－顺圆，3－－－逆圆(G00\G01\G02\G03)
    uint16_t AutoFlag:1;                      // 6自动运行标志 =1:自动运行中 =0:没有自动运行
    uint16_t ResetFlag:1;                     // 7 =1:按下服务按钮  =0:没有复位
    uint16_t DownProgramFlag:1;               // 8 ！=0:程序下载  =0:没有程序下载
    uint16_t DownOver:1;                      // 9 =1: 程序下载结束   =0: 程序下载中
    uint16_t Ret_Flag:1 ;                     // 10 已在参考点标志位
    uint16_t M30Stop: 1;                      // 11
    uint16_t MStop:1;                         // 12表示有DNC程序下传到DSP
    uint16_t EnableMRun:1;                    // 13DNC开始结束标志
    uint16_t AutoJud:1;                       // 14自动模式
    uint16_t StepEnd:1;                       // 15单段
//    uint16_t ThreeWordsFlag:1;                //三字符译码完成标志
//  uint16_t ret_finished_x:2;                    // 回参完成标志 = 0;=1;=-1
//  uint16_t ret_finished_y:2;                    // 回参完成标志 = 0;=1;=-1
//  uint16_t ret_finished_z:2;                    // 回参完成标志 = 0;=1;=-1
//  uint16_t ret_finished_a:2;                    // 回参完成标志 = 0;=1;=-1
//  uint16_t ret_finished_b:2;                    // 回参完成标志 = 0;=1;=-1
//  uint16_t ret_finished_c:2;                    // 回参完成标志 = 0;=1;=-1
};

union RunStatusFlagUN
{
    struct RunStatusFlagST bit;
    uint16_t all;
};
/*
struct RunStatusST
{
    int ret_finished_x;                 // 回参完成标志
    int ret_finished_y;                 // 回参完成标志
    int ret_finished_z;                 // 回参完成标志
    int ret_finished_a;                 // 回参完成标志
    int ret_finished_b;                 // 回参完成标志
    int ret_finished_c;                 // 回参完成标志
    int16_t ret_offset;                   // 回参进给偏移量
    int8_t RpdFlag;                       // 快速定位控制标志
    int  layer;                          //子程序调用的层数
    int  DdSel,DdSelBk;
    int  HandwheelFlag;
    int  ProNum,ProNumBk;
    int32_t   adjust_feed;          //调整后的指令速度
    int32_t   adjust_feed_flag;     // 匀速段速度调整标志，=1时需调整
    struct ALARM_REGS Alarm;    // 报警寄存器
    uint8_t   CorpSel;            // 坐标发送
    struct inter_in st_end;      // 起始终点坐标
    int     count_speed_min,circle_MDIstop_flag;
    int8_t    run_cnt;            // 预计算计数器
    int8_t    Angle_cnt;          //
    int8_t    G28Flag;
    struct int64 UserPara[48];          //用户变量
    int         JumpFlag;                       //程序跳转标志
    int         CountN;                     //跳转时的程序段号
    struct PUSHPOP SUBPRO[10];                  //子程序调用
    uint32_t    G04TimerCounter;              // G04暂停时间时间
    int        M_time;                      // M代码时间
    union  FLAG_REG ReturnFlag;
    int32_t  ReturnSpeed[6];                  // 回参考点子程序返回的速度值
//    int16_t  OutFPGAPulse[6];
    uint16_t Zero_pol;            // 零点信号电平
//  uint16_t SPILoadPulse;
    union RunStatusFlagUN RunStatusFlag;
 };*/
struct RunCommandST                    // IR201
{
    uint16_t AutoJud:1;                       // 0 自动模式
    uint16_t Type:5;                          // =1 回参考点 =2 手动方式 =4 编辑=8 JOG= 16 MDI
    uint16_t Pause: 1;                        // 6暂停
    uint16_t ModeDisable: 1;                  // 7禁止模式转换
    uint16_t MulX1: 1;                        // 8 X1
    uint16_t MulX10: 1;                       // 9 X10
    uint16_t MulX100: 1;                      // 10 X100
    uint16_t StepEnd:1;                       // 11单段
    uint16_t AutoPause: 1;                    // 12 自动暂停
    uint16_t M30Stop: 1;                      // 12 自动暂停
};
union RunCommandUN
{
    struct RunCommandST bit;
    uint16_t all;
};


struct RunStatusST
{
    int     ret_finished_x;                 // 回参完成标志
    int     ret_finished_y;                 // 回参完成标志
    int     ret_finished_z;                 // 回参完成标志
    int     ret_finished_a;                 // 回参完成标志
    int     ret_finished_b;                 // 回参完成标志
    int     ret_finished_c;                 // 回参完成标志
    int     ret_finished_w;                 // 回参完成标志
    int     ret_finished_u;                 // 回参完成标志
    int16_t   ret_offset;                 // 回参进给偏移量
    //uint16_t    retAxisNo;                      //指定的回参轴
    int8_t    RpdFlag;                        // 快速定位控制标志
    int     layer;                           //子程序调用的层数
    int     DdSel,DdSelBk;
    int     HandwheelFlag;
    int     ProNum,ProNumBk;
    int32_t   adjust_feed;          //调整后的指令速度
    int32_t   adjust_feed_flag;     // 匀速段速度调整标志，=1时需调整
    struct  ALARM_REGS Alarm;   // 报警寄存器
    uint8_t   CorpSel;            // 坐标发送
    struct  inter_in st_end;         // 起始终点坐标
    int     count_speed_min,circle_MDIstop_flag;
    int8_t    run_cnt;            // 预计算计数器
    int8_t    Angle_cnt;          //
    int8_t    G28Flag;
    struct INT64 UserPara[48];          //用户变量
    double UserDoublePara[48];
    int     JumpFlag;                       //程序跳转标志
    int     CountN;                     //跳转时的程序段号
    struct PUSHPOP SUBPRO[10];                  //子程序调用
    uint32_t  G04TimerCounter;              // G04暂停时间时间
    int     M_time;                     // M代码时间
    union   FLAG_REG ReturnFlag;
    int32_t   ReturnSpeed[6];                 // 回参考点子程序返回的速度值
//    int16_t  OutFPGAPulse[6];
    uint16_t Zero_pol;            // 零点信号电平
    uint16_t RealSpeed;           // 实时速度
    uint16_t CycleTimes;       // 循环次数
//  uint16_t SPILoadPulse;
    union RunStatusFlagUN RunStatusFlag;
    union RunCommandUN RunCommand;

    /* 文件下载标志位 */
    uint16_t file_download_flag;
    /* 单行指令下载标志位 */
    uint16_t single_download_flag;
    /* EPG 的标志位，1是运行，0是不运行 */
    int16_t EPGFlag;
    /* BCP 的标志位，1是烧录，0是不烧录 */
    int16_t BCPFlag;

    /* 要下传的代码数量 */
    int32_t codeBytes;
    /**
     * 当前运行指令的类型，分为运动型和其它。运动指令为 1，其它指令为 0
     * */
    int16_t cmdType;
    /* 当前指令是否完成。0：没有完成，1：完成 */
    int16_t isCmdCompelete;

    /* 保存代码的缓冲区 */
    char* codeBuffer;

    /* AIT 要等待的输出点，-1 代表没有，>0代表要等待的输出点 */
    int16_t AITWaitSeq;
    /* AIT 指令要等待的电平 */
    int16_t AITLevel;

    /* RST 复位标志位 */
    int16_t RSTFlag;

 };


extern struct RunStatusST RunstatusRegs;
struct InterCirST
{
    struct INT64 half_length;           // 圆弧过半圆高16位标志
    struct inter_in_cir st_end_cir;         // 圆弧输入
    uint16_t cir_first;
    int8_t cir_half,cir_flag;             // 圆弧插补到位标志,过半圆标志
    uint8_t zuo_you;                      // 优劣弧
    int32_t remx,remy,remx1,remy1;            // 圆弧插补余数累加器
};
extern struct AccDecVarStruct SpeedRegs;
extern struct LineInterST LineInterRegs;
extern struct InterCirST CirCleInterRegs;
struct PostionST
{
    struct inter_pos AbsPos;    // 零件坐标
    struct inter_pos DuiDao;        // 对刀时的位置
    struct inter_pos real_pos;          // 轴实时位置
    struct inter_pos CurCo;             //当前用的坐标系
    struct inter_pos real_pos1;         // 轴实时位置1
    struct inter_pos real_pos2;         // 轴实时位置2
    struct inter_pos real_pos3;         // 轴实时位置3
    struct inter_pos real_pos4;         // 轴实时位置3
    struct inter_pos LinPos;    // 零件坐标
    struct inter_pos real_posbk,out_imp,out_imp_bk;// 定义脉冲输出数和前次位置备份
    struct inter_pos last_real_posbk; //记录上上次的轴位置，用于计算正切轴的角度变化 zjq
    struct inter_pos offset_temp;       // 偏置距离临时值
    struct inter_pos out_add;   // 电子齿轮比累加寄存器
    int32_t  posX_recode[200];
    int32_t  posY_recode[200];
};
extern struct PostionST PostionRegs;

struct CToMDataStruct
{
    uint16_t IOdata[120];
    /* 写指针 */
    uint16_t writeIdx;

    struct inter_pos AbsPos;    // 零件坐标
    struct inter_pos DuiDao;        // 对刀时的位置
    struct inter_pos real_pos;          // 轴实时位置
    struct inter_pos CurCo;             //当前用的坐标系
    uint16_t ProNum;
    uint32_t RealSpeed;
    uint16_t CycleTimes;       // 循环次数
    uint16_t  MCode1;
    uint16_t  MCode2;
    uint16_t  M_time;           // 暂停时间
    /* 输出信息 */
//    int16_t message[120];
};


struct MToCDataStruct
{
    uint16_t IOdata[100];
    uint16_t CNCData[512];
};
//struct OPFMDataST
//{
//    char *ReadFMAddr;
//    char *WriteFMAddr;
//    uint16_t ReadWriteFMFlag;
//    uint16_t CmdWrPtr;
//    uint16_t CmdRdPtr;
//    uint16_t ReadWriteCmd[4];
//    uint16_t ReadFMFirstAddr;
//    uint16_t WriteFMFirstAddr;
//    uint16_t ReadFMLength;
//    uint16_t WriteFMLength;
//    uint16_t ReadFMPtr;
//    uint16_t WriteFMPtr;
//};
//extern struct OPFMDataST ReadWriteFMRegs;
/*struct CToMDataStruct
{
    uint16_t IOdata[32];
    struct inter_pos AbsPos;    // 零件坐标
    struct inter_pos DuiDao;        // 对刀时的位置
    struct inter_pos real_pos;          // 轴实时位置
    struct inter_pos CurCo;             //当前用的坐标系
    // 加工状态运行、停止、暂停、继续、复位、单步
    // 行号
    // 输入状态
    // 输出状态
    // 驱动器状态
     union SERVO_REG servo; // 伺服器状态
    // M状态
};
struct CToMCodeStruct
{
    uint16_t Data[2048];
};*/

struct STEPS
{   uint16_t X;
    uint16_t Y;
    uint16_t Z;
    uint16_t A;
    uint16_t B;
    uint16_t C;
    uint16_t W;
    uint16_t U;
    uint16_t valid;
};


struct ThreeWordsST
{
    int32_t X[100];
    int32_t Y[100];
    int32_t Z[100];
    int32_t A[100];
    int32_t B[100];
    int32_t C[100];
    int32_t W[100];
    int32_t U[100];
    uint16_t PosNumX;
    uint16_t PosNumY;
    uint16_t PosNumZ;
    uint16_t PosNumA;
    uint16_t PosNumB;
    uint16_t PosNumC;
    uint16_t PosNumW;
    uint16_t PosNumU;

    uint16_t Mode; //运动方式 G00 G01 G02 G03等
    uint8_t BGMFlag;  //各轴开始运动命令标志位
    uint8_t MONFlag;
//    uint8_t STMFlag;  //各轴停止运动命令标志位
    uint8_t ProModeFlag;  //绝对值/相对值编程标志位
//    uint8_t RPGFlag;  //相对值编程标志位
//    uint8_t AMPFlag;  //各轴完成运动标志位
    uint8_t VICFlag;  //坐标系选择标志位
    uint32_t MoveSpeed; //快速移动的速度
    uint32_t ChabuMoveSpeed;
    double ChabuMoveSpeedBeilv;
    int32_t I[100];
    int32_t J[100];
    int32_t K[100];
    int32_t R[100];
    uint32_t VACSpeed;//插补加速度
    uint32_t MACSpeed;//快速移动加速度
    uint16_t IOLow;  // IO低16位输出
    uint16_t IOHigh; // IO高16位输出
    uint16_t iIOLow;   // IO低16位输入
    uint16_t iIOHigh;    // IO高16位输出
//    uint8_t ThreeWordCodeFlag;

};
extern struct ThreeWordsST ThreeWordsReg;
extern struct BPMParams bpmParams;


extern struct INT64 add64(struct INT64 temp1,struct INT64 temp2);
extern struct INT64 div64(struct INT64 temp1,int32_t temp2);
extern struct INT64 div64X64(struct INT64 x1,struct INT64 x2);
extern struct INT64 mul64(int32_t x1,int32_t x2) ;
extern struct INT64 mul64X32(struct INT64 x1,int32_t x2);
extern int32_t sqr32(int32_t temp);
extern int32_t sqr64(struct INT64 temp);
extern struct INT64 sub64(struct INT64 temp1,struct INT64 temp2);
extern void speed_pre(void);
extern void speed_control(void);
extern void speed_feed_length(struct SpeedSTRRegs *in);
extern int8_t InCode2Cs(void);
extern int8_t CsRegNext(void) ;
extern void Cs2Bs(void);
extern void Bs2As(void);
extern void DB2Bs();
extern void EncodeMain();

extern void CLEARABC();
//extern void InitSysCtrl();
//extern void InitGpio();
////extern void InitPieCtrl();
//extern void InitPieVectTable();

extern void Shun_Cir();
extern void Ni_Cir();
//extern void DD();
extern int RC(void);
extern void myPC(void); //zjq 10.20
extern void RCCompInit(void);
extern uint16_t RpdRun(uint16_t cmdRun);
extern uint16_t LineChabu(uint16_t cmdRun);
extern uint16_t ShunNi_Cir(uint16_t cmdRun);
extern void ReadCncCode();
extern void ReadCncParm();
extern void ReadCncCoord();

//extern void ehand();
extern struct RTNDATA Cal_JoinPoint(int i,int G,int G4x,struct S_Line Line1,struct S_Line Line2,int32_t d,char* OutStr);
//extern void SPI_loop(void);
extern void InitPara(void);
extern void InitPara_c(void);
extern void JointsChabu(void);
//extern void DealWith_4096us(void);

extern void EncodeThreeWordMain();

extern uint16_t AutoRun(void);

//extern void AutoRun(void);
//extern void CorpConv(void);
//extern void EGear(void);
extern void Impls_Out(int bit, int32_t pls);
extern uint16_t MyRpdRun2(uint16_t cmdRun);
extern uint16_t DD(void) ;
//extern void RetRefPoint(void);

#ifdef __cplusplus
}
#endif

#endif
