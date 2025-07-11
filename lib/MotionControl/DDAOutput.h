#ifndef __DDAOUTPUT_H__
#define __DDAOUTPUT_H__

#include <string.h>

#include "pmcsystem.h"
#include "InterpMotionBlock.h"
#include "variabledef.h"
#include "params.h"

class DDAOutput {
public:
    DDAOutput()
    {
        Init();
    }

    bool Init()
    {
         memset(accu, 0, sizeof(accu));
         memset(lp, 0, sizeof(lp));
         memset(lo, 0, sizeof(lo));
         return true;
    }

    /* 积分法求取输出 */
    bool Out(InterpMotionBlock* block)
    {
        int32_t out[8];
        /* 输出 */
        for (int i=0; i<8; ++i) {
            pmc_real_t cp = block->GetPosition(i);
            accu[i] += (cp - lp[i]);

            if (block->IsExecuteOk()) {
                /* 如果这是最后一段，加入一点补偿  */
                pmc_real_t delt = accu[i] - lo[i];
                if (delt >= 0) {
                    out[i] = delt + 0.01;
                } else {
                    out[i] = delt - 0.01;
                }
            } else {
                out[i] = accu[i] - lo[i];
            }

            lo[i] += out[i];
            lp[i] = cp;
        }

//        PostionRegs.real_pos.pos_x += out[0];
//        PostionRegs.real_pos.pos_y += out[1];
//        PostionRegs.real_pos.pos_z += out[2];
//        PostionRegs.real_pos.pos_a += out[3];
//        PostionRegs.real_pos.pos_b += out[4];
//        PostionRegs.real_pos.pos_c += out[5];
//        PostionRegs.real_pos.pos_w += out[6];
//        PostionRegs.real_pos.pos_u += out[7];

        for(int j = 0; j < 8; j++){
            if((out[j] > 0 && nowIfLimit[j * 2] != 1 && STMAxises[j] != 1 && InterplastLimit[j * 2] != 1 )
                    ||(out[j] < 0 && nowIfLimit[j * 2 + 1] != 1 && STMAxises[j] != 1 && InterplastLimit[j * 2 + 1] != 1)) {
               /* 如果要输出正向脉冲并且当前没有正限位且没有STM停轴*//* 如果要输出负向脉冲并且当前没有负限位且没有STM停轴 */
                *g_params_axisRealPositionPtr[j] += out[j];
            }else if(nowIfLimit[j * 2] == 1){
                /* 记录历史限位 */
                InterplastLimit[j * 2] = 1;
            }else if(nowIfLimit[j * 2 + 1] == 1){
                InterplastLimit[j * 2 + 1] = 1;
            }
        }

        return true;
    }

private:
    pmc_real_t accu[8];
    pmc_real_t lp[8];
    int32_t lo[8];
};



#endif