#ifndef LIB_PARAMSCPP_H_
#define LIB_PARAMSCPP_H_


/**
 * 保存了一些只能用于 .cpp 中文件的全局参数
 */
#include "ProgrammerControl/ProgrammerBulk.h"
#include "SpeedFilterQueue.h"
#include "ThreadSafeQueue.h"
#include "RespParser.h"

extern ProgrammerBulk g_programmerBulk;
extern SpeedFilterQueue g_speedFilterQueueA;
extern SpeedFilterQueue g_speedFilterQueueB;
extern SpeedFilterQueue g_speedFilterQueueC;
extern SpeedFilterQueue g_speedFilterQueueD;
extern SpeedFilterQueue g_speedFilterQueueE;
extern SpeedFilterQueue g_speedFilterQueueF;
extern SpeedFilterQueue g_speedFilterQueueG;
extern SpeedFilterQueue g_speedFilterQueueH;
extern ThreadSafeQueue<RespMessage> threadSafeQueue;
extern ThreadSafeQueue<cstr>  threadSafeQueue2; 
#endif /* LIB_PARAMSCPP_H_ */