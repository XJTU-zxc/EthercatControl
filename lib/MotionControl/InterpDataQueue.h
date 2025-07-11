#ifndef __INTERPDATAQUEUE_H__
#define __INTERPDATAQUEUE_H__

#include <math.h>
#include <limits.h>

#include "InterpData.h"
#include "variabledef.h"

enum {
    INTERP_DATA_QUEUE_MAX_SIZE = 20,       /* 最大存储插补段数 */
    MAX_LOOK_AHEAD_SIZE = 10,               /* 最大前瞻段数 */
    SPEED_LOOK_AHEAD_MAX_ITER = 25,         /* 二分法最大迭代次数 */
};

class InterpDataQueue {
public:
    InterpDataQueue() : head(0), len(0), free(INTERP_DATA_QUEUE_MAX_SIZE), isLookAhead(false){}

    /*
     * @brief 加入一段直线插补
     * */
    bool Push(pmc_real_t vs, pmc_real_t vm, pmc_real_t am, pmc_real_t ve, pmc_real_t dist,
        pmc_real_t ua, pmc_real_t ub, pmc_real_t uc, pmc_real_t ud,
        pmc_real_t ue, pmc_real_t uf, pmc_real_t ug, pmc_real_t uh)
    {
        if (this->free == 0) {
            return false;
        }
        this->isLookAhead = true;
        /* 放入尾部 */
        int i = (this->head + this->len) % INTERP_DATA_QUEUE_MAX_SIZE;

        interpDatas[i].data.lineData.vs = vs;
        interpDatas[i].data.lineData.vm = vm;
        interpDatas[i].data.lineData.am = am;
        interpDatas[i].data.lineData.ve = ve;
        interpDatas[i].data.lineData.dist = dist;
        interpDatas[i].data.lineData.unitVec[0] = ua;
        interpDatas[i].data.lineData.unitVec[1] = ub;
        interpDatas[i].data.lineData.unitVec[2] = uc;
        interpDatas[i].data.lineData.unitVec[3] = ud;
        interpDatas[i].data.lineData.unitVec[4] = ue;
        interpDatas[i].data.lineData.unitVec[5] = uf;
        interpDatas[i].data.lineData.unitVec[6] = ug;
        interpDatas[i].data.lineData.unitVec[7] = uh;
        interpDatas[i].type = INTERP_TYPE_LINE;

        ++this->len;
        --this->free;

        this->isLookAhead = false;

        return true;
    }

    /*
     * @brief 加入一段圆弧插补
     * */
    bool Push(pmc_real_t vm, pmc_real_t am, int32_t r, int32_t q, int32_t d)
    {
        if (this->free == 0) {
            return false;
        }
        this->isLookAhead = true;

        int i = (this->head + this->len) % INTERP_DATA_QUEUE_MAX_SIZE;

        interpDatas[i].data.circleData.vm = vm;
        interpDatas[i].data.circleData.am = am;
        interpDatas[i].data.circleData.r = r;
        interpDatas[i].data.circleData.q = q;
        interpDatas[i].data.circleData.d = d;

        interpDatas[i].type = INTERP_TYPE_CIRCLE;
        ++this->len;
        --this->free;

        return true;
    }

    /*
     * @brief 加入一段圆弧插补
     * */
    bool Push(pmc_real_t vm, pmc_real_t am, int32_t edx, int32_t edy, int32_t i, int32_t j, int type)
    {
        if (this->free == 0) {
            return false;
        }
        this->isLookAhead = true;

        int k = (this->head + this->len) % INTERP_DATA_QUEUE_MAX_SIZE;

        interpDatas[k].data.circleData.vm = vm;
        interpDatas[i].data.circleData.am = am;
        interpDatas[k].data.circleData.edx = edx;
        interpDatas[k].data.circleData.edy = edy;
        interpDatas[k].data.circleData.i = i;
        interpDatas[k].data.circleData.j = j;
        interpDatas[k].data.circleData.circle_type = type;

        interpDatas[k].type = INTERP_TYPE_CIRCLE;
        ++this->len;
        --this->free;

        return true;
    }

    InterpData* Pop()
    {
        if (this->len == 0) {
            return NULL;
        }
        InterpData* idata = &interpDatas[this->head];
        this->head = (this->head + 1) % INTERP_DATA_QUEUE_MAX_SIZE;

        --this->len;
        ++this->free;

        return idata;
    }

    bool IsLookAhead()
    {
        return this->isLookAhead;
    }
    
    bool IsFull()
    {
        return this->free == 0;
    }

    int16_t LeisureLen()
    {
        return this->free;
    }

    bool IsEmpty()
    {
        return this->len == 0;
    }

    void Clear()
    {
        this->head = 0;
        this->len = 0;
        this->free = INTERP_DATA_QUEUE_MAX_SIZE;
    }

private:
    pmc_real_t GetLTheory(pmc_real_t v1, pmc_real_t v2,
        pmc_real_t j, pmc_real_t nm, pmc_real_t ts)
    {
        pmc_real_t t = ts;
        pmc_real_t dv = abs(v1 - v2);
        pmc_real_t m1 = dv / (nm*j*pow(t, 2)) - nm;
        pmc_real_t m2 = sqrt(dv / (j*pow(t,2)));

        pmc_real_t n1 = m1 > 0 ? nm : (int)ceil(m2);   // ceil()函数作用：向上取整
        pmc_real_t n2 = m1 > 0 ? (int)ceil(m1) : 0;

        pmc_real_t a2 = (dv - (n1 - 1) * (n1 + n2 -1) * j * pow(t,2)) / (2*n1 + n2 -1) / t;
        pmc_real_t a2_ = v1 < v2 ? a2 : -a2;

        pmc_real_t j_ = v1 < v2 ? j : -j;    

        pmc_real_t n1_2 = pow(n1, 2);
        pmc_real_t n1_3 = pow(n1, 3);
        pmc_real_t n1n2 = n1 * n2;
        pmc_real_t n2_2 = pow(n2, 2);

        pmc_real_t res = (2.0*n1 + n2) * v1 * t +
                0.5 * a2_ * (4*n1_2 + 4*n1n2 + n2_2 - 2*n1 - n2) * pow(t, 2) +
                0.5 * (2*n1_3 + 3*n1_2*n2 + n1*n2_2 - 4*n1_2 - 4*n1n2 - n2_2 + 2*n1 + n2) * j_ * pow(t,3);

        return res;
    }

    pmc_real_t GetVeTheory(pmc_real_t l, pmc_real_t vs, pmc_real_t vm,
        pmc_real_t j, pmc_real_t nm, pmc_real_t t, pmc_real_t mpe)
    {
        /* 如果 l < lmin，该段以 vs 做直线运动。取 ve = vs */
        
        pmc_real_t lth = this->GetLTheory(vs, vm, j, nm, t);
        if (lth < l) {
            return vm;
        }
        pmc_real_t ve = vs;
        pmc_real_t vmin = vs;
        pmc_real_t vmax = vm;
        pmc_real_t vmid;
        /* 二分法重新计算 vm ，并且 ve=vm，为保证插补周期，有最大迭代次数限制 */
        for (int i=0; i<SPEED_LOOK_AHEAD_MAX_ITER; ++i) {
            vmid = 0.5 * (vmin + vmax);
            lth = this->GetLTheory(vs, vmid, j, nm, t);
            if (abs(lth - l) < mpe) {
                ve = vmid;
                break;
            }
            else if (lth < l) {
                vmin = vmid;
                ve = vmid;
            } else {
                vmax = vmid;
            }
        }

        return ve;
    }

    bool LookAhead()
    {
        /* 只有一个，不进行速度前瞻 */
        if (this->len <= 1) {
            return true;
        }

        int tail = (this->head + this->len -1) % INTERP_DATA_QUEUE_MAX_SIZE;

        int myLen = this->len;
        if (myLen > MAX_LOOK_AHEAD_SIZE) {
            myLen = MAX_LOOK_AHEAD_SIZE;
        }
        int myHead = (tail - myLen + 1 + INTERP_DATA_QUEUE_MAX_SIZE) % INTERP_DATA_QUEUE_MAX_SIZE;

        /* 前向 */
        double last_ve = interpDatas[myHead].type==INTERP_TYPE_CIRCLE? 0 : interpDatas[myHead].data.lineData.vs;
        /* 还没修改好 */
        for (int i=myHead, j=0; j<myLen-1; ++i, ++j) {
            int idx = i % INTERP_DATA_QUEUE_MAX_SIZE;
            int next_idx = (i + 1) % INTERP_DATA_QUEUE_MAX_SIZE;
            if (interpDatas[idx].type == INTERP_TYPE_CIRCLE) {
                /* 不对圆弧插补做速度前瞻 */
                last_ve = 0;
                continue;
            } else if (interpDatas[next_idx].type == INTERP_TYPE_CIRCLE) {
                interpDatas[idx].data.lineData.vs = last_ve;
                interpDatas[idx].data.lineData.ve = 0;
                last_ve = 0;
                continue;
            }

            interpDatas[idx].data.lineData.vs = last_ve;

            float nm = interpDatas[idx].data.lineData.am / (PMC_J*PMC_T);
            last_ve = GetVeTheory(interpDatas[idx].data.lineData.dist, interpDatas[idx].data.lineData.vs,
                interpDatas[idx].data.lineData.vm, PMC_J, nm, PMC_T, InterpData::MPE());

            interpDatas[idx].data.lineData.ve = last_ve;
        }

        /* 反向 */
//        int last_idx = (head + len -1) % INTERP_DATA_QUEUE_MAX_SIZE;
        interpDatas[tail].data.lineData.vs = last_ve;
        last_ve = 0;
        for (int i=tail, j=0; j<myLen-1; --i, ++j) {
            int idx = (i + INTERP_DATA_QUEUE_MAX_SIZE) % INTERP_DATA_QUEUE_MAX_SIZE;
            int next_idx = (i + INTERP_DATA_QUEUE_MAX_SIZE - 1) % INTERP_DATA_QUEUE_MAX_SIZE;
            if (interpDatas[idx].type == INTERP_TYPE_CIRCLE) {
                /* 不对圆弧插补做速度前瞻 */
                last_ve = 0;
                continue;
            } else if (interpDatas[next_idx].type == INTERP_TYPE_CIRCLE) {
                interpDatas[idx].data.lineData.vs = 0;
                interpDatas[idx].data.lineData.ve = last_ve;
                last_ve = 0;
                continue;
            }

            interpDatas[idx].data.lineData.ve = last_ve;

            float nm = interpDatas[idx].data.lineData.am / (PMC_J*PMC_T);
            pmc_real_t ve = GetVeTheory(interpDatas[idx].data.lineData.dist, interpDatas[idx].data.lineData.ve,
                interpDatas[idx].data.lineData.vm, PMC_J, nm, PMC_T, InterpData::MPE());

            if (ve < interpDatas[idx].data.lineData.vs) {interpDatas[idx].data.lineData.vs = ve;}

            last_ve = interpDatas[idx].data.lineData.vs;
        }

//        /* 前向 */
//        double last_ve = interpDatas[this->head].data.lineData.vs;
//        for (int i=head, j=0; j<len-1; ++i, ++j) {
//            int idx = i % INTERP_DATA_QUEUE_MAX_SIZE;
//            interpDatas[idx].data.lineData.vs = last_ve;
//            last_ve = GetVeTheory(interpDatas[idx].data.lineData.dist, interpDatas[idx].data.lineData.vs,
//                interpDatas[idx].data.lineData.vm, PMC_J, PMC_Nm, PMC_T, InterpData::MPE());
//
//            interpDatas[idx].data.lineData.ve = last_ve;
//        }
//
//        /* 反向 */
//        int last_idx = (head + len -1) % INTERP_DATA_QUEUE_MAX_SIZE;
//        interpDatas[last_idx].data.lineData.vs = last_ve;
//        last_ve = 0;
//        for (int i=last_idx, j=0; j<len-1; --i, ++j) {
//            int idx = (i + INTERP_DATA_QUEUE_MAX_SIZE) % INTERP_DATA_QUEUE_MAX_SIZE;
//            interpDatas[idx].data.lineData.ve = last_ve;
//            pmc_real_t ve = GetVeTheory(interpDatas[idx].data.lineData.dist, interpDatas[idx].data.lineData.ve,
//                interpDatas[idx].data.lineData.vm, PMC_J, PMC_Nm, PMC_T, InterpData::MPE());
//
//            if (ve < interpDatas[idx].data.lineData.vs) {interpDatas[idx].data.lineData.vs = ve;}
//
//            last_ve = interpDatas[idx].data.lineData.vs;
//        }

        /*  */
        for (int i=myHead; i<myLen-1; ++i) {
            int idx = i % INTERP_DATA_QUEUE_MAX_SIZE;
            int next_idx = (i + 1) % INTERP_DATA_QUEUE_MAX_SIZE;

            if (interpDatas[idx].type == INTERP_TYPE_CIRCLE || interpDatas[next_idx].type == INTERP_TYPE_CIRCLE) {
                continue;
            }

            /* 计算最大速度限制 */
            pmc_real_t min_ve = interpDatas[idx].data.lineData.vm < interpDatas[next_idx].data.lineData.vm ?
                    interpDatas[idx].data.lineData.vm : interpDatas[next_idx].data.lineData.vm;

            /* 计算系统最大加速度限制 */
            for (int k=0; k<8; ++k) {
                pmc_real_t ku = abs((interpDatas[next_idx].data.lineData.unitVec[k] - interpDatas[idx].data.lineData.unitVec[k]));
                pmc_real_t v_temp = INT_MAX;
                if (ku != 0) {
                    v_temp = PMC_LIMIT_AM * PMC_T / ku;
                }
                if (v_temp < min_ve) {
                    min_ve = v_temp;
                }
            }

            if (min_ve < interpDatas[idx].data.lineData.ve) {
                interpDatas[idx].data.lineData.ve = min_ve;
                interpDatas[next_idx].data.lineData.vs = min_ve;
            }
        }

        return true;
    }

private:
    InterpData interpDatas[INTERP_DATA_QUEUE_MAX_SIZE];
    int16_t head;
    int16_t len;
    int16_t free;
    bool isLookAhead;
};


#endif