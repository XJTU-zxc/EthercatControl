#ifndef TIMER_H
#define TIMER_H 1


#include "variabledef.h"

void Impls_Out(uint16_t pls,uint16_t *addr);
//uint16_t readMemory(uint16_t* address);
void IPC_Loop();
void CorpConv(void);
uint16_t RetRefPoint(void);
uint16_t JOG(uint16_t Limit);
uint16_t MyJOG();
uint16_t MyJOG2();
uint16_t AutoRun(void);
int AccDecMain(int32_t S_obj_x, int32_t *S_last_x, int32_t *Ei_x, int GM_x, int *JJFlag);
uint16_t DD(void);
void delay(int n);
bool isChange(void); //判断轴的位置是否发生了变化  zjq
void DealWith_4096us(void);

#endif