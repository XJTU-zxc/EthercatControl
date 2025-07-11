#include "params.h"
#include "variabledef.h"
#include "util.h"
#include "compile.h"

/**
 * 指令参数
 * */
/* VSR 指令参数：插补运动速度倍率。范围在0-10之间 */
float g_params_vsr = 1.000;
/* MIT 指令参数：加减速轮廓控制参数，八个轴的参数独立。范围在 0.004-1.0之间 */
float g_params_mit[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
/* BGM 的参数。 0 代表不能运行，1代表电机能运行。0-9位代表 ABCDEFGHS */
int16_t g_params_bgm = 0;
/* AIT 的参数 */
int16_t g_params_commandline_ait_seq = -1; // 选择等待的 IO
int16_t g_params_commandline_ait_val = 0;  // AIT 等待高电平或者低电平
/* AMP 参数 */
int16_t g_params_commandline_amp = 0;
/* AMS 参数 */
/* EGM、EGR 参数 */
int16_t g_params_egm_enable = 0;                            // EGM 使能
int16_t g_params_egm[8] = {-1, -1, -1, -1, -1, -1, -1, -1}; // EGM 参数
float g_params_egr[8] = {0};                                // EGR 参数
float g_params_egr_compensate[8] = {0};                     // EGR 参数补偿
/* VTN 参数 */
int16_t g_params_vtn_enable = 0;             // VTN 使能
int16_t g_params_vtn_axis[3] = {-1, -1, -1}; // 指定二维平面的轴，第三个参数是正切轴wyf
double g_params_vtn_puls_per_degree = 0;     // VTN 指令中每转一度需要输出的脉冲 ,双精度浮点型wyf
double g_params_vtn_init_pos = 0;            // 正切轴为 0° 时的位置,双精度浮点型wyf
double circleParam[5] = {0, 0, 0, 0, 0};     // 圆心wyf  终点x, 终点y,  圆心x, 圆心y, 插补方向(顺时针2 逆时针 3)
double compensate_params = 0.0;              // 补偿wyf
extern double lastK = 0.0;                   // 初始弧度与上一次弧度wyf
extern bool init_tan_K_flag = false;
/* MOF MON 参数 */
int16_t g_params_mof = 0;
int16_t g_params_is_execute_prog = 0; // 程序是否执行
int16_t g_params_prog_counter = 0;    // 程序计数器

int32_t g_params_wtm_count = 0;                       // 暂停时间
int16_t g_params_motion_mode = 0;                     // 运动模式。0：普通；1：插补模式
int16_t g_params_interp_axis_enable = 0;              // 使能了插补的轴
int16_t g_params_tangent_available = 0;               // 是否可进行正切wyf
int16_t g_params_interp_is_running = 0;               // 是否正在进行插补运动
int16_t g_params_plate_interp_axis[3] = {-1, -1, -1}; // 二维平面插补轴

/* 急停参数 */
int16_t g_params_est = 0; // 0 没有急停， 1 停轴， 2 停轴停程序
/**
 * @brief TEC 指令参数：读取控制器没有接受命令的原因
 * */
int16_t g_params_tec = TEC_ERROR_NONE;

/* VLI 参数 */
int16_t g_params_vli[3];
int32_t g_params_vsp = 10000 * g_params_vsp_coeff; /* 插补速度 */
float g_params_vsp_coeff = 0.655738;               /* 插补速度系数 */
int32_t g_params_vac = 500;                        /* 插补加速度 */
float g_params_vac_coeff = 1;                      /* 插补加速度系数 */

/* 是否使能JOG */
int16_t g_params_jog_enable = 0;
uint16_t jogAxis = 0;
uint16_t axises[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 0,2,4,6..正转 1，3，5，7反转
uint16_t needBrake[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t AllAxisEnd = 0xffff;
int JogPhase = 0;
uint16_t NewJogCommand = 0;
/* 限位 */
uint16_t Limit = 0;
uint16_t nowIfLimit[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};      // 判断当前是否还有限位
uint16_t InterplastLimit[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 插补运动上一次限位记录
/* 回零Z脉冲 */
uint16_t g_params_z = 0;
/* STM参数 */
uint16_t g_params_stm = 0;                        // 0 不需要停；  1 所有轴全部停 ；  2 有个别轴要停
uint16_t STMAxises[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // 需要停的轴

/* flash烧写 */
uint16_t BCPBufferPtr = 0;
uint16_t BCPBuffer[BCPBufferSize] = {0}; // BCP缓存
uint16_t progNumTest = 0;                // 使用了多少个512缓冲区
// uint32_t codeCurAddr = Bzero_Sector6_start; // 当前的程序的next程序地址
uint16_t subProgramNum = 0; // 子程序的数量
const char *BCPOK = "BCPok";

/**
 *  其它参数
 * */
uint16_t isInterpProg = 0; // 当前flash读取的指令是否是插补指令

/* CPU 发送给 ARM 消息的消息结构 */
char g_cpu1_to_arm_message_init_buf[256]; // 1 k的初始化 buf
cstr g_message_cpu2arm;                   // 用这个给 arm 发消息

/* 编译模块 */
Compiler g_compiler;
Compiler *g_compilerPtr; // 用于给其它模块使用的指针，指向 g_compiler

uint16_t testZero = 0;
/**
 * @brief 全局 io 参数
 * */
/* 正限位 io */
uint16_t g_io_plimitIO = 0;
/* 负限位 io */
uint16_t g_io_nlimitIO = 0;
/* 某个轴是否正在运动 */
uint16_t g_axis_is_running = 0;
/* 点位运动轴最终要运动到的位置 */
MVXAxisParams g_axis_mvx_params[8] = {0};
int32_t *g_params_axisPositionPtr[8];     // EncodeRegs.AsReg 映射
int32_t *g_params_axisRealPositionPtr[8]; // PostionRegs.real_pos映射
int32_t *g_params_wpPositionPtr[8];       // PostionRegs.LinPos映射
int32_t *g_params_axisLastPulse[8];       // PostionRegs.real_posbk映射
int32_t *g_params_axisPulseData[8];       // PostionRegs.out_imp映射

uint16_t z_pulse_param = 0x0000; // 初始为高电平 zjq
uint16_t fsw_enable_flag = 0;    // fsw使能标志 zjq
uint16_t fsw_enable_flag_last_buf = 0;
uint16_t fid_enable_flag = 0;
uint16_t z_pulse_param_buf = 0x0000;

uint16_t g_bkc_flag = 0; // 间隙补偿flag
// zjq 10.20 螺距补偿相关
uint16_t g_dec_flag = 0;                                                         // 螺距补偿flag
uint16_t g_dfn_len = 128;                                                        // 定义正向螺距误差补偿数组最大标号
uint16_t g_drn_len = 128;                                                        // 定义负向螺距误差补偿数组最大标号
uint16_t g_dzn_len[8] = {1000, 20000, 30000, 40000, 10000, 20000, 30000, 40000}; // 8个轴的机械零点脉冲数组
uint16_t g_dit_param = 1;                                                        // 螺距补偿间隔距离   默认1不可为0
uint16_t g_luobu_arr[G_LUOBU_ARR_LEN];                                           // 测试用的螺补数组

uint16_t interpNum = 0; // 当前CPU2插补队列的条数
IpcLineData ipcLineData;
DataUnion dataUnion;
IpcCircleData ipcCircleData;

int GlobalParamsInit()
{
    // 螺补数组初始化
    for (int i = 0; i < G_LUOBU_ARR_LEN; i++)
    {
        g_luobu_arr[i] = i;
    }

    /* 初始化编译模块 */
    CompilerInit(&g_compiler);
    g_compilerPtr = &g_compiler;

    g_message_cpu2arm = CStringInit(g_cpu1_to_arm_message_init_buf, sizeof(g_cpu1_to_arm_message_init_buf));

    // 将指针指向 EncodeRegs.AsReg 的各个轴
    g_params_axisPositionPtr[0] = &EncodeRegs.AsReg.X;
    g_params_axisPositionPtr[1] = &EncodeRegs.AsReg.Y;
    g_params_axisPositionPtr[2] = &EncodeRegs.AsReg.Z;
    g_params_axisPositionPtr[3] = &EncodeRegs.AsReg.A;
    g_params_axisPositionPtr[4] = &EncodeRegs.AsReg.B;
    g_params_axisPositionPtr[5] = &EncodeRegs.AsReg.C;
    g_params_axisPositionPtr[6] = &EncodeRegs.AsReg.W;
    g_params_axisPositionPtr[7] = &EncodeRegs.AsReg.U;

    // 将指针指向 PostionRegs.real_pos 的各个轴
    g_params_axisRealPositionPtr[0] = &PostionRegs.real_pos.pos_x;
    g_params_axisRealPositionPtr[1] = &PostionRegs.real_pos.pos_y;
    g_params_axisRealPositionPtr[2] = &PostionRegs.real_pos.pos_z;
    g_params_axisRealPositionPtr[3] = &PostionRegs.real_pos.pos_a;
    g_params_axisRealPositionPtr[4] = &PostionRegs.real_pos.pos_b;
    g_params_axisRealPositionPtr[5] = &PostionRegs.real_pos.pos_c;
    g_params_axisRealPositionPtr[6] = &PostionRegs.real_pos.pos_w;
    g_params_axisRealPositionPtr[7] = &PostionRegs.real_pos.pos_u;

    // 将指针指向 PostionRegs.LinPos 的各个轴
    g_params_wpPositionPtr[0] = &PostionRegs.LinPos.pos_x;
    g_params_wpPositionPtr[1] = &PostionRegs.LinPos.pos_y;
    g_params_wpPositionPtr[2] = &PostionRegs.LinPos.pos_z;
    g_params_wpPositionPtr[3] = &PostionRegs.LinPos.pos_a;
    g_params_wpPositionPtr[4] = &PostionRegs.LinPos.pos_b;
    g_params_wpPositionPtr[5] = &PostionRegs.LinPos.pos_c;
    g_params_wpPositionPtr[6] = &PostionRegs.LinPos.pos_w;
    g_params_wpPositionPtr[7] = &PostionRegs.LinPos.pos_u;

    // 将指针指向 PostionRegs.real_posbk 的各个轴
    g_params_axisLastPulse[0] = &PostionRegs.real_posbk.pos_x;
    g_params_axisLastPulse[1] = &PostionRegs.real_posbk.pos_y;
    g_params_axisLastPulse[2] = &PostionRegs.real_posbk.pos_z;
    g_params_axisLastPulse[3] = &PostionRegs.real_posbk.pos_a;
    g_params_axisLastPulse[4] = &PostionRegs.real_posbk.pos_b;
    g_params_axisLastPulse[5] = &PostionRegs.real_posbk.pos_c;
    g_params_axisLastPulse[6] = &PostionRegs.real_posbk.pos_w;
    g_params_axisLastPulse[7] = &PostionRegs.real_posbk.pos_u;

    // 将指针指向 PostionRegs.out_imp 的各个轴
    g_params_axisPulseData[0] = &PostionRegs.out_imp.pos_x;
    g_params_axisPulseData[1] = &PostionRegs.out_imp.pos_y;
    g_params_axisPulseData[2] = &PostionRegs.out_imp.pos_z;
    g_params_axisPulseData[3] = &PostionRegs.out_imp.pos_a;
    g_params_axisPulseData[4] = &PostionRegs.out_imp.pos_b;
    g_params_axisPulseData[5] = &PostionRegs.out_imp.pos_c;
    g_params_axisPulseData[6] = &PostionRegs.out_imp.pos_w;
    g_params_axisPulseData[7] = &PostionRegs.out_imp.pos_u;

    return 0;
}
