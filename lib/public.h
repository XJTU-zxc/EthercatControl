#ifndef LIB_PUBLIC_H_
#define LIB_PUBLIC_H_


#include <stdint.h>

#ifdef __cplusplus
extern "C" {

#endif

#define AXIS_X x
#define AXIS_Y y
#define AXIS_Z z
#define AXIS_A a
#define AXIS_B b
#define AXIS_C c
#define AXIS_U u
#define AXIS_W w

#define ENCODE_MASK 0x8000
#define ENCODE_RESEVER_MASK 0x7FFF
#define ENCODE_MAX_VALUE 32768

/* 储存编码器的数据结构  */
typedef struct EncodeNode {
    /* 上一次读取的编码器值 */
    int32_t last;

    /* 当前编码器值 */
    int32_t curr;

    /* 上一次从编码器读取的值 */
    int16_t lastEncode;
} EncodeNode;


typedef struct SFEncode {
    EncodeNode x;
    EncodeNode y;
    EncodeNode z;
    EncodeNode a;
    EncodeNode b;
    EncodeNode c;
    EncodeNode u;
    EncodeNode w;
} SFEncode;

/* 得到指定轴编码器的值 */
#define GetEncodeValue(ins, axis) (ins.axis.curr)

/* 获取指定轴的速度 */
#define GetEncodeSpeed(ins, axis) (ins.axis.curr - ins.axis.last)

/* 设置编码器的值 */
#define SetEncodeVal(ins, axis, value) do { \
    int32_t real = value & ENCODE_RESEVER_MASK;     \
    int32_t incr = 0;                               \
    int32_t diff = 0;                               \
    int32_t last = ins.axis.lastEncode;             \
                                                    \
    ins.axis.last = ins.axis.curr;                  \
                                                    \
    diff = abs(real - ins.axis.lastEncode);         \
                                                    \
    if (real >= last) {                             \
        /* 反转 */                                    \
        if (diff > ENCODE_MAX_VALUE/2) {            \
            incr = (real - ins.axis.lastEncode - ENCODE_MAX_VALUE) % ENCODE_MAX_VALUE;  \
        }                                                                               \
        else {  /* 正转 */                                                                \
            incr = (real - ins.axis.lastEncode + ENCODE_MAX_VALUE) % ENCODE_MAX_VALUE;  \
        }                                                                               \
    } else {                                                                            \
        /* 正转 */                                                                        \
        if (diff > ENCODE_MAX_VALUE/2) {                                                \
            incr = (real - ins.axis.lastEncode + ENCODE_MAX_VALUE) % ENCODE_MAX_VALUE;  \
        }                                                                               \
        else {  /* 正转 */                                                                \
            incr = (real - ins.axis.lastEncode - ENCODE_MAX_VALUE) % ENCODE_MAX_VALUE;  \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    ins.axis.curr += incr;                                                              \
    ins.axis.lastEncode = real;                                                         \
} while(0);

/* 重新设置软件编码器的值 */
#define ResetEncodeVal(ins, axis, value) do {   \
    ins.axis.last = value;                      \
    ins.axis.curr = value;                      \
}while(0);

/* 通过读取当前硬件编码器的值来初始化软件编码器的值 */
#define InitEncode(ins, axis, initVal) do {     \
    ins.axis.last = 0;                      \
    ins.axis.curr = 0;                      \
    ins.axis.lastEncode = (initVal & ENCODE_RESEVER_MASK);          \
} while(0);


/* 初始化 SFEncode */
void SFEncodeInit(SFEncode* encode);


typedef struct RobotCore {
    /* 编码器 */
    SFEncode encode;

    /* 软件版本号 */
    int32_t softVerison;

    /* 硬件版本号 */
    int32_t hdVerison;
} RobotCore;

/* 初始化编码器 */
void RobotCoreInitEncode(RobotCore* core);

/* 读取编码器读数 */
void RobotCoreReadEncode(RobotCore* core);


/* 初始化 RobotCore */
void RobotCoreInit(RobotCore* core);


extern RobotCore robotCore;


#ifdef __cplusplus
}
#endif

#endif /* LIB_PUBLIC_H_ */

