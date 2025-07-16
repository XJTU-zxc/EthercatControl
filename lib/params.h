#ifndef LIB_PARAMS_H_
#define LIB_PARAMS_H_

#include <stdint.h>
#include <stdbool.h>
#include "RespParser.h"

struct Compiler;
#ifdef __cplusplus
extern "C"
{
#endif

#define MaxInterpNum 250
#define G_LUOBU_ARR_LEN 100
#define BCPBufferSize 512
    /* 记录 MVA 和 MVR 指令的参数 */
    typedef struct MVXAxisParams
    {
        /* 0：没有指令；1：MVA；2：MVR */
        int16_t type;
        /* 值 */
        int32_t val;
    } MVXAxisParams;

    /* 硬限位软限位 JOG检查 */
    typedef struct JOGLimitCheck
    {
        int16_t hardLimited;
        int16_t softLimited;
    } JOGLimitCheck;

    /* 用于IPC数据传输的结构体 */
    typedef struct IpcCircleData
    {
        double vm;
        double am;
        int circle_type;
        int32_t edx;
        int32_t edy;
        int32_t i;
        int32_t j;
    } IpcCircleData;

    /* 用于IPC数据传输的结构体 */
    typedef struct IpcLineData
    {
        double vs;
        double vm;
        double ve;
        double am;
        double dist;
        double unitVec[8];
        // pmc_real_t j;
        // pmc_real_t nm;
        // pmc_real_t ts;
    } IpcLineData;

    /* 用于IPC数据传输的结构体 */
    typedef union DataUnion
    {
        struct IpcLineData l;
        struct IpcCircleData c;
        uint16_t array[100];
    } DataUnion;

    /* TEC 指令可选枚举值 */
    enum
    {
        TEC_ERROR_NONE = 0,   // 没有错误，指令正常
        TEC_ERROR_VSR = -100, // VSR 指令设置参数失败。参数设置的范围在 0-10 之间
        TEC_ERROR_MIT = -101, // MIT 指令设置参数失败。参数设置的范围在 0.004-1 之间
        TEC_ERROR_BGM = -102, // BGM 错误。
        TEC_ERROR_MVA = -103, // MVA 错误。
        TEC_ERROR_MVR = -104, // MVR 错误。
        TEC_ERROR_AIT = -105, // MVR 错误。
        TEC_ERROR_EGR = -106, //
        TEC_ERROR_EGM = -107,
        TEC_ERROR_DPE = -108,
        TEC_ERROR_DPC = -109,
        TEC_ERROR_MSP = -110,
        TEC_ERROR_MAC = -111,
        TEC_ERROR_CLB = -112,
        TEC_ERROR_PROGRAM = -113, // 语法错误
        TEC_ERROR_SET_BUSY = -115,
        TEC_ERROR_GENERAL = -116, // 不想思考是什么错误，就用这个
        TEC_ERROR_IO_BUSY = -117,
        TEC_ERROR_INTERP_FULL = -118,   // 插补队列已满
        TEC_ERROR_FID = -119,           // FID参数和FSW不匹配
        TEC_ERROR_PERSIST = -120,       // 烧写出错
        TCE_ERROR_NOT_UP_COMMAND = -121 // 不能是上位机命令
    };

    /**
     * 全局参数管理
     * */
    extern uint16_t testZero; // 测试

    extern double lastK; // 初始弧度与上一次弧度wyf
    extern bool init_tan_K_flag; // 正切刀初始位置计算位标志wyf

    extern float g_params_vsr;
    extern float g_params_mit[8];
    extern int16_t g_params_bgm;
    extern int16_t g_params_tec;
    extern uint16_t g_io_plimitIO;
    extern uint16_t g_io_nlimitIO;
    extern uint16_t g_axis_is_running;
    extern MVXAxisParams g_axis_mvx_params[8];
    extern int32_t *g_params_axisPositionPtr[8];
    extern int32_t *g_params_axisRealPositionPtr[8];
    extern int32_t *g_params_wpPositionPtr[8];
    extern int32_t *g_params_axisLastPulse[8];
    extern int32_t *g_params_axisPulseData[8];

    extern int16_t g_params_commandline_ait_seq;
    extern int16_t g_params_commandline_ait_val;
    extern int16_t g_params_commandline_amp;
    extern int16_t g_params_egm_enable;
    extern int16_t g_params_egm[8];
    extern float g_params_egr[8];
    extern float g_params_egr_compensate[8];
    extern int16_t g_params_vtn_enable;
    extern int16_t g_params_vtn_axis[3];
    extern double g_params_vtn_puls_per_degree; // 双精度浮点型wyf
    extern double g_params_vtn_init_pos;        // 双精度浮点型wyf
    extern double circleParam[5];               // 圆心全局wyf
    extern double lastK;
    extern bool init_tan_K_flag; // 正切刀初始位置计算位标志wyf

    extern int16_t g_params_is_execute_prog;
    extern int16_t g_params_prog_counter;

    extern int32_t g_params_wtm_count;
    extern int16_t g_params_motion_mode;        // 运动模式。0：普通；1：插补模式
    extern int16_t g_params_interp_axis_enable; // 使能了插补的轴
    extern int16_t g_params_tangent_available;  // 正切标志wyf
    extern double compensate_params;            // 补偿系数wyf 用于VTN指令，输出小于1的脉冲会丢失该脉冲，需要补偿
    extern int16_t g_params_interp_is_running;  // 是否正在进行插补运动
    extern int16_t g_params_plate_interp_axis[3];

    extern int32_t g_params_vsp;     /* 插补速度 */
    extern float g_params_vsp_coeff; /* 插补速度系数 */
    extern int32_t g_params_vac;     /* 插补加速度 */
    extern float g_params_vac_coeff; /* 插补加速度系数 */

    extern int16_t g_params_mof;
    extern char *g_message_cpu2arm;
    extern struct Compiler *g_compilerPtr;
    extern int16_t g_params_est;

    extern int16_t g_params_jog_enable;
    extern uint16_t jogAxis;
    extern uint16_t Limit;
    extern uint16_t axises[16];
    extern uint16_t needBrake[16];
    extern uint16_t AllAxisEnd;
    extern int JogPhase;
    extern uint16_t NewJogCommand;
    extern uint16_t nowIfLimit[16];
    extern uint16_t InterplastLimit[16];

    extern uint16_t g_params_stm;
    extern uint16_t STMAxises[8];

    /* 回零相关 */
    extern uint16_t z_pulse_param;            // z脉冲 zjq
    extern uint16_t fsw_enable_flag;          // fsw使能标志 zjq
    extern uint16_t fsw_enable_flag_last_buf; // 缓存的上一个fsw使能标志 zjq
    extern uint16_t fid_enable_flag;          // fid使能标志 zjq
    extern uint16_t z_pulse_param_buf;        // z脉冲缓存

    extern uint16_t g_params_z;

    extern uint16_t g_bkc_flag; // 间隙补偿flag
    // zjq 10.20 螺距补偿相关
    extern uint16_t g_dec_flag;                   // 螺距补偿flag
    extern uint16_t g_dfn_len;                    // 定义正向螺距误差补偿数组最大标号
    extern uint16_t g_drn_len;                    // 定义负向螺距误差补偿数组最大标号
    extern uint16_t g_dzn_len[8];                 // 8个轴的机械零点脉冲数组
    extern uint16_t g_dit_param;                  // 补偿间隔脉冲
    extern uint16_t g_luobu_arr[G_LUOBU_ARR_LEN]; // 测试用的螺补数组

    extern uint16_t progNumTest;
    extern uint32_t codeCurAddr;
    extern uint16_t BCPBuffer[BCPBufferSize];
    extern uint16_t BCPBufferPtr;
    extern uint16_t subProgramNum;
    extern const char *BCPOK;
    extern uint16_t isInterpProg;
    extern uint16_t interpNum;

    extern uint16_t CPU1Send[100];
    extern uint16_t CPU1read[100];
    extern DataUnion dataUnion;
    extern IpcLineData ipcLineData;
    extern IpcCircleData ipcCircleData;

    /**
     * 全局参数初始化。一些需要初始化的变量在这个函数中初始化
     * */
    int GlobalParamsInit();

#ifdef __cplusplus
}
#endif

#endif /* LIB_PARAMS_H_ */
