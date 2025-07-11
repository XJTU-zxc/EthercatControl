#ifdef __cplusplus
extern "C"
{
#endif
#include "stdint.h"
#include "RespParser.h"

extern struct ControlReg gloabal_ctr_reg;  // 全局寄存器

struct ControlReg{
    char ir_reg[100];   // IO 寄存器
    void* dm_reg[100];   // 数据寄存器
};


int saveProgramApi(const char* buf, int buf_size);
int loadProgramApi(char* buf);

/**
 * @brief 检查命令行等待
 * */
int IsCommandLineWait();

/**
 * @brief 检查命令行等待并改变状态
 * */
int CheckCommandLineWait();

/**
 * @brief 执行 BGM 指令
 * */
int ExecuteBGMCommand();

/**
 * @brief 电子齿轮
 * */
int ExecuteEGear();

/**
 * @brief 切线控制
 * */
int TangentialControl();

/**
 * @brief 执行一条程序
 * */
int ExecuteOneProgram();


/**
 * @brief 执行一条flash程序
 * */
int ExecuteOneFlashProgram();

/**
 * @brief flash初始化与擦写
 * */
void flashInitAndErase();


/*
 * @brief 执行急停操作
 * */
int ExcuteEstCommand();

/*
 * @brief 给执行器添加插补数据
 * */
int PushData();

/*
 * @brief 处理三指令
 * */
int handleThreeWords(RespMessage msg);


struct IR_BITS {  // bits  description
   uint16_t BIT00:     1;
   uint16_t BIT01:     1;
   uint16_t BIT02:     1;
   uint16_t BIT03:     1;
   uint16_t BIT04:     1;
   uint16_t BIT05:     1;
   uint16_t BIT06:     1;
   uint16_t BIT07:     1;
   uint16_t BIT08:     1;
   uint16_t BIT09:     1;
   uint16_t BIT010:    1;
   uint16_t BIT011:    1;
   uint16_t BIT012:    1;
   uint16_t BIT013:    1;
   uint16_t BIT014:    1;
   uint16_t BIT015:    1;
};

union IR_REG {
   uint16_t            all;
   struct IR_BITS    bit;
};

// 用于读取输入输出共64个IO的结构体,之前使用了IR_REG[0,1,10,11],其余未使用
struct DataStorage{
    union IR_REG IR_Reg[15];
};
extern struct DataStorage dataStorage;

void Init();

#ifdef __cplusplus
}
#endif


