#ifndef __SCURVEPLAN_H__
#define __SCURVEPLAN_H__

#include <stdint.h>

/**
 * @brief 加速段参数结构体
 */
typedef struct AccelerationParams {
    /* ++ 匀+ -+ 段或者 +- 匀- -- 段的时间 */
    // int t1, t2, t3;
    int32_t n1, n2;
    /* 加速度 */
    double a;
    /* 加加速度 */
    double j;
    /* 插补周期 */
    double ts;
    /* 起始速度 */
    double v1;
} AccelerationParams;

/**
 * @brief 用于保存运动过程中的输出数据
 */
typedef struct SCurveOutput {
    /* 是否插补完毕 */
    bool isOK;
    /* 实时速度 */
    double vreal;
    /* 实时运动距离 */
    double sreal;
    /* 输出 */
    double last_sreal;
} SCurveOutput;

/**
 * @brief 加减速规划需要用到的输入
 */
typedef struct SCurveInput {
    /* 起始速度 */
    double vs;
    /* 终点速度 */
    double ve;
    /* 最大进给速度 */
    double vm;
    /* 需要加工的段长 */
    double l;
    /* 加加速度 */
    double j;
    /* 最大加加速插补周期数，该参数决定最大加速度。目前加速段和减速段加速度相同 */
    int32_t nm;
    /* 插补周期,单位 ms */
    double ts;
} SCurveInput;

/*
*   S 型曲线加减速结构体，用于实现加减速规划，速度前瞻
*/
typedef struct SCurveParams {
    /* 速度规划的输入参数 */
    SCurveInput si;
    /* 加速段参数 */
    AccelerationParams ap1;
    /* 减速段参数 */
    AccelerationParams ap2;
    /* 输出数据 */
    SCurveOutput so;

    /* 加减速各个段的插补周期数 */
    int32_t Tx[9];
    /* 当前所在段 */
    int32_t ni;
} SCurveParams;

SCurveParams* SCurveParamsInit(SCurveParams* sp);

/*
*   求取加减速过程的理论长度（不包括匀速过程），论文公式见函数实现处注释
*   @params v1 初始速度
*   @params v2 终点速度
*   @params j  加加速度
*   @params nm 最大加加速插补周期数
*   @params ts 插补周期, 单位 us
*
*   @return 加减速理论长度
*/
double SCurveAccelerationLength(double v1, double v2, double j, double nm, double ts);

/*
*   速度前瞻。求取衔接点速度用于 S 型曲线速度规划
*   @params l  待加工段长
*   @params v1 初始速度
*   @params v2 终点速度
*   @params j  加加速度
*   @params nm 最大加加速插补周期数
*   @params ts 插补周期, 单位 us
*/
double SpeedLookAhead(SCurveParams** params, int nb, double err);

/*
*   给定参数，进行 S 型曲线加减速规划
*/
int SCurvePlan(SCurveParams* sp,
    double l, double vs, double vm, double ve, double j, double nm, double ts);

/*
*   执行 S 型曲线加减速规划
*   @return 是否执行完
*/
bool SCurveExecute(SCurveParams* params);



#endif