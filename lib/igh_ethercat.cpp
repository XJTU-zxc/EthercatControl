#include "igh_ethercat.h"
#include "params.h"
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <mutex>
#include <thread>
#include <chrono>

extern EthercatMaster *curIgHInstance;
/* Mutex for ec_read and ec_write */
std::mutex mtx;

/* quit igh_flag */
volatile bool g_quit = false;
/* data for update and read*/
int act_status[SLAVE_NUM], act_mode[SLAVE_NUM], act_pos[SLAVE_NUM], act_v[SLAVE_NUM];
/* CSP Command Configuration*/
// const int32_t step_limit = 2500; // 每次最大改变量
// const int16_t allow_error = 10; // 允许的误差

// EtherCAT Configuration
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};
static ec_domain_t *domain = NULL;
static ec_domain_state_t domain_state = {};
static struct _SlaveConfig
{
    ec_slave_config_t *sc;
    ec_slave_config_state_t sc_state;
} slave_config[SLAVE_NUM];

/* Offset for PDO entries */
static struct _SlaveOffset
{
    unsigned int ctrl_word;
    unsigned int op_mode;
    unsigned int target_position;
    unsigned int target_velocity;

    unsigned int error_code;
    unsigned int status_word;
    unsigned int op_mode_display;
    unsigned int actual_position;
    unsigned int actual_velocity;
} slave_offset[SLAVE_NUM];

struct _SlaveInfo
{
    uint32_t alias;
    uint32_t position;
    uint32_t vendor_id;
    uint32_t product_code;
};

// PDO entry registration
const static ec_pdo_entry_reg_t domain_regs[] = {
    /* Slave 0 */
    {SLAVE_0, VID_PID, 0x6040, 0, &slave_offset[0].ctrl_word},
    {SLAVE_0, VID_PID, 0x6060, 0, &slave_offset[0].op_mode},
    {SLAVE_0, VID_PID, 0x607A, 0, &slave_offset[0].target_position},
    {SLAVE_0, VID_PID, 0x60FF, 0, &slave_offset[0].target_velocity},
    {SLAVE_0, VID_PID, 0x603F, 0, &slave_offset[0].error_code},
    {SLAVE_0, VID_PID, 0x6041, 0, &slave_offset[0].status_word},
    {SLAVE_0, VID_PID, 0x6061, 0, &slave_offset[0].op_mode_display},
    {SLAVE_0, VID_PID, 0x6064, 0, &slave_offset[0].actual_position},
    {SLAVE_0, VID_PID, 0x606C, 0, &slave_offset[0].actual_velocity},
    /* Slave 1 */
    {SLAVE_1, VID_PID, 0x6040, 0, &slave_offset[1].ctrl_word},
    {SLAVE_1, VID_PID, 0x6060, 0, &slave_offset[1].op_mode},
    {SLAVE_1, VID_PID, 0x607A, 0, &slave_offset[1].target_position},
    {SLAVE_1, VID_PID, 0x60FF, 0, &slave_offset[1].target_velocity},
    {SLAVE_1, VID_PID, 0x603F, 0, &slave_offset[1].error_code},
    {SLAVE_1, VID_PID, 0x6041, 0, &slave_offset[1].status_word},
    {SLAVE_1, VID_PID, 0x6061, 0, &slave_offset[1].op_mode_display},
    {SLAVE_1, VID_PID, 0x6064, 0, &slave_offset[1].actual_position},
    {SLAVE_1, VID_PID, 0x606C, 0, &slave_offset[1].actual_velocity},
    /* Slave 2 */
    {SLAVE_2, VID_PID, 0x6040, 0, &slave_offset[2].ctrl_word},
    {SLAVE_2, VID_PID, 0x6060, 0, &slave_offset[2].op_mode},
    {SLAVE_2, VID_PID, 0x607A, 0, &slave_offset[2].target_position},
    {SLAVE_2, VID_PID, 0x60FF, 0, &slave_offset[2].target_velocity},
    {SLAVE_2, VID_PID, 0x603F, 0, &slave_offset[2].error_code},
    {SLAVE_2, VID_PID, 0x6041, 0, &slave_offset[2].status_word},
    {SLAVE_2, VID_PID, 0x6061, 0, &slave_offset[2].op_mode_display},
    {SLAVE_2, VID_PID, 0x6064, 0, &slave_offset[2].actual_position},
    {SLAVE_2, VID_PID, 0x606C, 0, &slave_offset[2].actual_velocity},
    /* Slave 3 */
    {SLAVE_3, VID_PID, 0x6040, 0, &slave_offset[3].ctrl_word},
    {SLAVE_3, VID_PID, 0x6060, 0, &slave_offset[3].op_mode},
    {SLAVE_3, VID_PID, 0x607A, 0, &slave_offset[3].target_position},
    {SLAVE_3, VID_PID, 0x60FF, 0, &slave_offset[3].target_velocity},
    {SLAVE_3, VID_PID, 0x603F, 0, &slave_offset[3].error_code},
    {SLAVE_3, VID_PID, 0x6041, 0, &slave_offset[3].status_word},
    {SLAVE_3, VID_PID, 0x6061, 0, &slave_offset[3].op_mode_display},
    {SLAVE_3, VID_PID, 0x6064, 0, &slave_offset[3].actual_position},
    {SLAVE_3, VID_PID, 0x606C, 0, &slave_offset[3].actual_velocity},
    /* Slave 4 */
    {SLAVE_4, VID_PID, 0x6040, 0, &slave_offset[4].ctrl_word},
    {SLAVE_4, VID_PID, 0x6060, 0, &slave_offset[4].op_mode},
    {SLAVE_4, VID_PID, 0x607A, 0, &slave_offset[4].target_position},
    {SLAVE_4, VID_PID, 0x60FF, 0, &slave_offset[4].target_velocity},
    {SLAVE_4, VID_PID, 0x603F, 0, &slave_offset[4].error_code},
    {SLAVE_4, VID_PID, 0x6041, 0, &slave_offset[4].status_word},
    {SLAVE_4, VID_PID, 0x6061, 0, &slave_offset[4].op_mode_display},
    {SLAVE_4, VID_PID, 0x6064, 0, &slave_offset[4].actual_position},
    {SLAVE_4, VID_PID, 0x606C, 0, &slave_offset[4].actual_velocity},
    /* Slave 5 */
    {SLAVE_5, VID_PID, 0x6040, 0, &slave_offset[5].ctrl_word},
    {SLAVE_5, VID_PID, 0x6060, 0, &slave_offset[5].op_mode},
    {SLAVE_5, VID_PID, 0x607A, 0, &slave_offset[5].target_position},
    {SLAVE_5, VID_PID, 0x60FF, 0, &slave_offset[5].target_velocity},
    {SLAVE_5, VID_PID, 0x603F, 0, &slave_offset[5].error_code},
    {SLAVE_5, VID_PID, 0x6041, 0, &slave_offset[5].status_word},
    {SLAVE_5, VID_PID, 0x6061, 0, &slave_offset[5].op_mode_display},
    {SLAVE_5, VID_PID, 0x6064, 0, &slave_offset[5].actual_position},
    {SLAVE_5, VID_PID, 0x606C, 0, &slave_offset[5].actual_velocity},
    /* Slave 6 */
    {SLAVE_6, VID_PID, 0x6040, 0, &slave_offset[6].ctrl_word},
    {SLAVE_6, VID_PID, 0x6060, 0, &slave_offset[6].op_mode},
    {SLAVE_6, VID_PID, 0x607A, 0, &slave_offset[6].target_position},
    {SLAVE_6, VID_PID, 0x60FF, 0, &slave_offset[6].target_velocity},
    {SLAVE_6, VID_PID, 0x603F, 0, &slave_offset[6].error_code},
    {SLAVE_6, VID_PID, 0x6041, 0, &slave_offset[6].status_word},
    {SLAVE_6, VID_PID, 0x6061, 0, &slave_offset[6].op_mode_display},
    {SLAVE_6, VID_PID, 0x6064, 0, &slave_offset[6].actual_position},
    {}};

/*Config PDOs*/
static ec_pdo_entry_info_t slave_pdo_entries[] = {
    /*RxPdo 0x1600*/
    {0x6040, 0x00, 16}, // constrol word, UINT
    {0x6060, 0x00, 8},  // op mode, SINT
    {0x607A, 0x00, 32}, // target position, DINT
    {0x60FF, 0x00, 32}, // target velocity, DINT
    /*TxPdo 0x1A00*/
    {0x603F, 0x00, 16}, // error code, UINT
    {0x6041, 0x00, 16}, // status word, UINT
    {0x6061, 0x00, 8},  // op mode display, SINT
    {0x6064, 0x00, 32}, // actual position, DINT
    {0x606C, 0x00, 32}, // actual velocity, DINT
};

static ec_pdo_info_t slave_pdos[] = {
    // RxPdo
    {0x1600, 4, slave_pdo_entries + 0},
    // TxPdo
    {0x1A00, 5, slave_pdo_entries + 4},
};

static ec_sync_info_t slave_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},           // SM0
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},            // SM1
    {2, EC_DIR_OUTPUT, 1, slave_pdos + 0, EC_WD_DISABLE}, // SM2
    {3, EC_DIR_INPUT, 1, slave_pdos + 1, EC_WD_DISABLE},  // SM3
    {0xff}};

static struct _SlaveInfo slave_info[] = {
    {SLAVE_0, VID_PID},
    {SLAVE_1, VID_PID},
    {SLAVE_2, VID_PID},
    {SLAVE_3, VID_PID},
    {SLAVE_4, VID_PID},
    {SLAVE_5, VID_PID},
    {SLAVE_6, VID_PID}};

/* Implementation of EthercatMaster constructor and destructor */
EthercatMaster::EthercatMaster()
{
    // Constructor implementation (optional)
}

EthercatMaster::~EthercatMaster()
{
    ReleaseMaster();
    if (munlockall() == -1)
    {
        perror("munlockall failed");
    }
}

int EthercatMaster::initialize()
{
    int ret = -1;
    curIgHInstance = this;
    signal(SIGINT, EthercatMaster::sig_handle);
    printf("Request master ...\n");
    /* Request master */
    master = ecrt_request_master(0);
    if (!master)
    {
        printf("--> main: Request master failed.\n");
        return -1;
    }
    printf("--> main: Request master success.\n");

    /* Create domain */
    printf("Create domain...\n");
    domain = ecrt_master_create_domain(master);
    if (!domain)
    {
        printf("--> main: Create domain failed.\n");
        ReleaseMaster();
        return -1;
    }
    printf("--> main: Create domain success.\n");

    /* Get slave configuration */
    for (int i = 0; i < SLAVE_NUM; i++)
    {
        printf("Get slaver configuration.\n");
        slave_config[i].sc = ecrt_master_slave_config(master, slave_info[i].alias, slave_info[i].position, slave_info[i].vendor_id,
                                                      slave_info[i].product_code);
        if (!slave_config[i].sc)
        {
            printf("--> main: Get slave configuration failed.\n");
            ReleaseMaster();
            return -1;
        }
    }
    printf("--> main: Get slave configuration success.\n");

    /* Configuring PDO */
    for (int i = 0; i < SLAVE_NUM; i++)
    {
        printf("Configure PDOs...\n");
        ret = ecrt_slave_config_pdos(slave_config[i].sc, EC_END, slave_syncs);
        if (ret != 0)
        {
            printf("--> main: Configuration PDO failed.\n");
            ReleaseMaster();
            return -1;
        }
    }
    printf("--> main: Configuration PDO success.\n");

    /* Registers a bunch of PDO entries for a domain. */
    printf("Registers a bunch of PDO entries for a domain...\n");
    ret = ecrt_domain_reg_pdo_entry_list(domain, domain_regs);
    if (ret != 0)
    {
        printf("--> main: Failed to register bunch of PDO entries for domain.\n");
        ReleaseMaster();
        return -1;
    }
    printf("--> main: success to register bunch of PDO entries for domain.\n");

    /* Activate EtherCAT master */
    printf("Activate EtherCAT master...\n");
    ret = ecrt_master_activate(master);
    if (ret < 0)
    {
        printf("--> main: Activate master failed.\n");
        ReleaseMaster();
        return -1;
    }
    printf("--> main: Activate master success.\n");

    /* Get Pointer to the process data memory */
    printf("Get Pointer to the process data memory ...\n");
    domain_pd = ecrt_domain_data(domain);
    if (!domain_pd)
    {
        printf("--> main: Get pointer to the process data memory failed.\n");
        ReleaseMaster();
        return -1;
    }
    printf("--> main: Get pointer to the process data memory success.\n");

    /* Set priority */
    struct sched_param param = {};
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    printf("Using priority %i.", param.sched_priority);
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
    {
        perror("--> main: Warning: Failed to set priority.\n");
    }
    else
    {
        printf("Using priority %i.\n", param.sched_priority);
    }

    printf("--> main: Enter cycle task now...\n");

    // 锁定当前进程的内存，防止这些内存页被换出到交换空间
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
        perror("mlockall failed");
        return -1;
    }
    stack_prefault();
    return 0;
}

void EthercatMaster::start()
{
}

// 发送和接收过程数据
void EthercatMaster::cyclic_task()
{
    ecrt_master_receive(master);
    ecrt_domain_process(domain);
    check_domain_state();
    check_master_state();
    check_slave_config_states();
}

void EthercatMaster::cyclic_task(int8_t p)
{
    ecrt_master_receive(master);
    ecrt_domain_process(domain);
    check_domain_state();
    check_master_state();
    check_slave_config_state(p);
}

void EthercatMaster::sig_handle(int signal)
{
    if (curIgHInstance)
    {
        printf("Signal %d received. Stopping servos and releasing resources...\n", signal);
        g_quit = true;
        curIgHInstance->stop_servo();    // 停止伺服驱动
        curIgHInstance->ReleaseMaster(); // 释放 EtherCAT 主站资源
        exit(0);                         // 正常退出
    }
}
void EthercatMaster::stop_servo()
{
    ecrt_master_receive(master);
    ecrt_domain_process(domain);

    for (int i = 0; i < SLAVE_NUM; i++)
    {
        EC_WRITE_U16(domain_pd + slave_offset[i].ctrl_word, 256); // 停止伺服
    }

    ecrt_domain_queue(domain);
    ecrt_master_send(master);
}

void EthercatMaster::check_domain_state()
{
    ec_domain_state_t ds;
    ecrt_domain_state(domain, &ds);
    if (ds.working_counter != domain_state.working_counter)
    {
        printf("--> check_domain_state: Domain: WC %u.\n", ds.working_counter);
    }
    if (ds.wc_state != domain_state.wc_state)
    {
        printf("--> check_domain_state: Domain: State %u.\n", ds.wc_state);
    }
    domain_state = ds;
}

void EthercatMaster::check_master_state()
{
    ec_master_state_t ms;
    ecrt_master_state(master, &ms);
    if (ms.slaves_responding != master_state.slaves_responding)
    {
        printf("--> check_master_state: %u slave(s).\n", ms.slaves_responding);
    }
    if (ms.al_states != master_state.al_states)
    {
        printf("--> check_master_state: AL states: 0x%02X.\n", ms.al_states);
    }
    if (ms.link_up != master_state.link_up)
    {
        printf("--> check_master_state: Link is %s.\n", ms.link_up ? "up" : "down");
    }
    master_state = ms;
}

void EthercatMaster::check_slave_config_states()
{
    ec_slave_config_state_t s;
    for (int i = 0; i < SLAVE_NUM; i++)
    {
        memset(&s, 0, sizeof(s));
        ecrt_slave_config_state(slave_config[i].sc, &s);
        // Check and report slave state...
        if (s.al_state != slave_config[i].sc_state.al_state)
        {
            printf("--> check_slave_config_state: slave[%d]: State 0x%02X.\n", i, s.al_state);
        }

        if (s.online != slave_config[i].sc_state.online)
        {
            printf("--> check_slave_config_state: slave[%d]: %s.\n", i, s.online ? "online" : "offline");
        }
        if (s.operational != slave_config[i].sc_state.operational)
        {
            printf("--> check_slave_config_state: slave[%d]: %soperational.\n", i, s.operational ? "" : "Not ");
        }
        slave_config[i].sc_state = s;
    }
}

void EthercatMaster::check_slave_config_state(int8_t p)
{
    ec_slave_config_state_t s;
    memset(&s, 0, sizeof(s));
    ecrt_slave_config_state(slave_config[p].sc, &s);
    // Check and report slave state...
    if (s.al_state != slave_config[p].sc_state.al_state)
    {
        printf("--> check_slave_config_state: slave[%d]: State 0x%02X.\n", p, s.al_state);
    }
    if (s.online != slave_config[p].sc_state.online)
    {
        printf("--> check_slave_config_state: slave[%d]: %s.\n", p, s.online ? "online" : "offline");
    }
    if (s.operational != slave_config[p].sc_state.operational)
    {
        printf("--> check_slave_config_state: slave[%d]: %soperational.\n", p, s.operational ? "" : "Not ");
    }
    slave_config[p].sc_state = s;
}

void EthercatMaster::stack_prefault(void)
{
    unsigned char dummy[MAX_SAFE_STACK];
    memset(dummy, 0, MAX_SAFE_STACK);
}

void EthercatMaster::ReleaseMaster()
{
    /* Releases EtherCAT master */
    if (master)
    {
        ecrt_release_master(master); // 释放 EtherCAT 主站
        printf("--> main: Release master.\n");
        master = NULL; // 防止悬空指针
    }
}

void EthercatMaster::trans2OP() {
    
}

void EthercatMaster::ec_write_targetpos(int8_t p)
{
    int ctrl_word = 0;
    int status_word = act_status[p];
    int32_t target_position = *g_params_axisRealPositionPtr[p];
    if (status_word & 0x0008) // error
    {
        ctrl_word = 0x0080;
    }
    else if (status_word & 0x0040) // switch on disabled
    {
        ctrl_word = 0x0006;
    }
    else if ((status_word & 0x0007) == 0x1) // ready to swith on
    {
        ctrl_word = 0x0007;
    }
    else if ((status_word & 0x0007) == 0x3) // switched on
    {
        ctrl_word = 0x000F;
    }
    else if ((status_word & 0x0007) == 0x7) // operation enabled
    {
        ctrl_word = 0x000F;
    }
    else
    {
        ctrl_word = 0x0000;
    }
    EC_WRITE_U16(domain_pd + slave_offset[p].op_mode, MOTOR_MODEL_CSP);
    EC_WRITE_U16(domain_pd + slave_offset[p].ctrl_word, ctrl_word);
    EC_WRITE_S32(domain_pd + slave_offset[p].target_position, target_position*100);
}

void EthercatMaster::set_all_position(void)
{
    // 确保此操作不与主循环冲突
    std::lock_guard<std::mutex> lock(mtx);
    cyclic_task();
    // 获取所有从站的状态字、操作模式、实际位置和实际速度
    for (int i = 0; i < SLAVE_NUM; i++)
    {
        act_status[i] = EC_READ_U16(domain_pd + slave_offset[i].status_word);
        act_mode[i] = EC_READ_U8(domain_pd + slave_offset[i].op_mode_display);
        act_pos[i] = EC_READ_S32(domain_pd + slave_offset[i].actual_position);
        act_v[i] = EC_READ_S16(domain_pd + slave_offset[i].actual_velocity);
    }
    // ToDo: delete
    printf("slaves positions: ");
    for (int i = 0; i < SLAVE_NUM; i++) {
        printf("%d", act_pos[i]);
        if (i < SLAVE_NUM - 1) {
            printf(", ");
        }
    }
    printf("\n");

    // 设置每个从站的目标位置
    for (int i = 0; i < SLAVE_NUM; i++)
    {
        ec_write_targetpos(i);
    }
    /* Send process data */
    ecrt_domain_queue(domain);
    ecrt_master_send(master);
}

// TODO: Implement the handleUpdate function
int EthercatMaster::handleUpdate()
{
    return 0;
}

struct timespec timespec_add(struct timespec time1, struct timespec time2)
{
    struct timespec result;

    if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC)
    {
        result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
    }
    else
    {
        result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
    }

    return result;
}