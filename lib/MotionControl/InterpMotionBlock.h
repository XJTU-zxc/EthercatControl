#ifndef __INTERPMOTIONBLOCK_H__
#define __INTERPMOTIONBLOCK_H__

#include <string.h>

#include "pmcsystem.h"
#include "InterpData.h"
#include "SCurvePlanner.h"

class InterpMotionBlock
{
public:
    InterpMotionBlock() : hasPlan(false) {}

    bool Plan()
    {
        float nm = am / (PMC_J * PMC_T); // am = (default)500
        // int SCurvePlanner::Plan(double l, double vs, double vm, double ve, double j, double nm, double ts)
        // @param l: 合运动距离，VLI P1, P2，传入dist = sqrt(P1^2 + P2^2)
        // @param vs: 起点速度，default=0
        // @param vm: 最大速度，default=10000*0.655738=6557.38
        // @param ve: 终点速度，default=0
        // @param j: 加加速度，default=5000
        // @param nm: 最大加加速度周期数，default=am / (PMC_J * PMC_T)=500/(5000*0.001)=100
        // @param ts: 插补周期，default=0.001
        sp.Plan(dist, vs, vm, ve, PMC_J, nm, PMC_T);
        hasPlan = true;
        return true;
    }

    bool Execute()
    {
        if (this->type == INTERP_TYPE_LINE)
        {
            /* 执行直线插补 */
            if (sp.IsExecuteOk())
            {
                return true;
            }
            sp.Execute();
            return sp.IsExecuteOk();
        }
        else
        {
            /* 执行圆弧插补 */
            // int rc = CircleInterp(1, this);
            // return rc == 0;
            return false; // 圆弧插补未实现
        }
    }

    bool IsExecuteOk()
    {
        return sp.IsExecuteOk();
    }

    bool HasPlan()
    {
        return this->hasPlan;
    }

    bool Init(InterpData *idata)
    {
        if (idata->type == INTERP_TYPE_LINE)
        {
            /* 直线插补 */
            vs = idata->data.lineData.vs;
            vm = idata->data.lineData.vm;
            am = idata->data.lineData.am;
            ve = idata->data.lineData.ve;
            dist = idata->data.lineData.dist;
            memcpy(unitVec, idata->data.lineData.unitVec, sizeof(unitVec));
        }
        else
        {
            /* 圆弧插补 */
            vm = idata->data.circleData.vm;
            am = idata->data.circleData.am;
            edx = idata->data.circleData.edx;
            edy = idata->data.circleData.edy;
            i = idata->data.circleData.i;
            j = idata->data.circleData.j;
            circle_type = idata->data.circleData.circle_type;
        }

        type = idata->type;
        hasPlan = false;

        return true;
    }

    pmc_real_t GetPosition(int i)
    {
        return sp.GetPosition() * unitVec[i];
    }

    int Type()
    {
        return type;
    }

private:
public:
    /* 直线插补数据 */
    pmc_real_t vs;
    pmc_real_t vm;
    pmc_real_t ve;
    pmc_real_t dist;
    pmc_real_t unitVec[8];

    /* 圆弧插补数据 */
    int16_t circle_type;
    int32_t edx;
    int32_t edy;
    int32_t i;
    int32_t j;

    float am;
    /* 插补类型 */
    int type;

    bool hasPlan;
    /* sp */
    SCurvePlanner sp;
};

#endif
