#ifndef IGH_ETHERCAT_H
#define IGH_ETHERCAT_H

#include "util.h"
#include "ecrt.h"
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string>
/** Task period in ns. */
#define PERIOD_NS (1000000)
#define MAX_SAFE_STACK (8 * 1024)
#define CLOCK_TO_USE CLOCK_REALTIME

/* Constants */
#define NSEC_PER_SEC (1000000000)
#define FREQUENCY (NSEC_PER_SEC / PERIOD_NS) // 周期1ms

/* Operation model */
#define MOTOR_MODEL_CSP 8 // CSP同步周期位置模式0x08
#define MOTOR_MODEL_PP 1  // PP轮廓位置模式0x01
// #define MOTOR_MODEL_CSV 9  // CSV同步周期速度模式0x09

/* Control word */
// #define CTRL_HALF_BREAK 256 // 停止运行0x100
// #define CTRL_CLEAR_ERROR 128  // 清除错误标志0x80
// #define CTRL_PRE_ENABLE 6   // 预置使能0x06
// #define CTRL_ENABLE_OPERATION 7  // 使能运行0x07
// #define CTRL_OP 15  // 运行模式0x0F

// process data
static uint8_t *domain_pd = NULL;
#define SLAVE_NUM 7
#define SLAVE_0 0, 0
#define SLAVE_1 0, 1
#define SLAVE_2 0, 2
#define SLAVE_3 0, 3
#define SLAVE_4 0, 4
#define SLAVE_5 0, 5
#define SLAVE_6 0, 6
#define VID_PID 0x0000066F, 0x60380004

struct timespec timespec_add(struct timespec time1, struct timespec time2);

class EthercatMaster
{
public:
    EthercatMaster();
    ~EthercatMaster();
    void stack_prefault();
    void ReleaseMaster();
    int initialize();
    void start();
    void cyclic_task();
    void cyclic_task(int8_t p);
    void stop_servo();
    static void sig_handle(int signal);
    int handleUpdate();
    void set_all_position(void);
    void ec_write_targetpos(int8_t p);
    void trans2OP();

private:
    void check_domain_state();
    void check_master_state();
    void check_slave_config_states();
    void check_slave_config_state(int8_t p);
};

#endif // IGH_ETHERCAT_H
