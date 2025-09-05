#include "ScurvePlan.h"
#include "pmcsystem.h"
#include <math.h>
#include <stdio.h>
double SCurveAccelerationLength(double v1, double v2, double j, double nm, double ts)
{
    /* 由于速度的单位为 m/s ，而插补周期单位为 us，所以需要将 us 换成 s*/
    double t = ts;

    double dv = abs(v1 - v2);
    double m1 = dv / (nm * j * pow(t, 2)) - nm;
    double m2 = sqrt(dv / (j * pow(t, 2)));

    int32_t n1 = m1 > 0 ? nm : (int32_t)ceil(m2); // ceil()函数作用：向上取整
    int32_t n2 = m1 > 0 ? (int32_t)ceil(m1) : 0;

    double a2 = (dv - (n1 - 1) * (n1 + n2 - 1) * j * pow(t, 2)) / (2 * n1 + n2 - 1) / t;
    double a2_ = v1 < v2 ? a2 : -a2;

    double j_ = v1 < v2 ? j : -j;

    double res = (2.0 * n1 + n2) * v1 * t +
                 0.5 * a2_ * (4 * pow(n1, 2) + 4 * n1 * n2 + pow(n2, 2) - 2 * n1 - n2) * pow(t, 2) +
                 0.5 * (2 * pow(n1, 3) + 3 * pow(n1, 2) * n2 + n1 * pow(n2, 2) - 4 * pow(n1, 2) - 4 * n1 * n2 - pow(n2, 2) + 2 * n1 + n2) * j_ * pow(t, 3);

    return res;
}

double SCurveAccelerationLength(AccelerationParams *ap)
{
    double t = ap->ts;
    int32_t n1 = ap->n1;
    int32_t n2 = ap->n2;
    double a2_ = ap->a;
    double j_ = ap->j;
    double v1 = ap->v1;

    /* 加速或减速段长的计算 */
    double res = (2.0 * n1 + n2) * v1 * t +
                 0.5 * a2_ * (4 * pow(n1, 2) + 4 * n1 * n2 + pow(n2, 2) - 2 * n1 - n2) * pow(t, 2) +
                 0.5 * (2 * pow(n1, 3) + 3 * pow(n1, 2) * n2 + n1 * pow(n2, 2) - 4 * pow(n1, 2) - 4 * n1 * n2 - pow(n2, 2) + 2 * n1 + n2) * j_ * pow(t, 3);

    return res;
}

/**
 * @brief 计算加速或减速段的参数，结果保存在 ap 中
 * 参数包括：T1，T2，T3，a_，j_
 */
// default: vs=0, vm=10000*0.655738, ve=0, j=5000, nm=100, ts=0.001
int SCurveGetAccelerationParams(AccelerationParams *ap,
                                double v1, double v2, double j, double nm, double ts)
{
    double t = ts;

    double dv = abs(v1 - v2);
    double m1 = dv / (nm * j * pow(t, 2)) - nm; // 19900
    double m2 = sqrt(dv / (j * pow(t, 2)));

    int32_t n1 = m1 > 0 ? nm : (int32_t)ceil(m2); // ceil()函数作用：向上取整，dv=40时候，n1=90
    int32_t n2 = m1 > 0 ? (int32_t)ceil(m1) : 0;

    double a2 = (dv - (n1 - 1) * (n1 + n2 - 1) * j * pow(t, 2)) / (2 * n1 + n2 - 1) / t;
    // double a2 = m1 > 0 ? (nm * (PMC_J * PMC_T)) : ((dv - (n1 - 1) * (n1 - 1) * j * pow(t, 2)) / (2 * n1 - 1) / t);

    // ap->t1 = n1 + 1;
    // ap->t2 = n1 + n2 + 1;
    // ap->t3 = 2 * n1 + n2;
    ap->n1 = n1; // 100
    ap->n2 = n2; // 19900
    ap->a = v1 < v2 ? a2 : -a2;
    ap->j = v1 < v2 ? j : -j;  // 5000
    ap->ts = t;  // 0.001
    ap->v1 = v1; // 0

    return 0;
}

int SCurvePlanCheckParams(double l, double vs, double vm, double ve, double j, double nm, double ts)
{

    return 0;
}

// default: vs=0, vm=10000*0.655738, ve=0, j=5000, nm=100, ts=0.001
int SCurvePlan(SCurveParams *sp,
               double l, double vs, double vm, double ve, double j, double nm, double ts)
{

    /**
     * @brief 首先进行参数的合理性检查
     */
    // SCurvePlanCheckParams();

    /**
     * @brief 加速段的参数计算
     */
    SCurveGetAccelerationParams(&sp->ap1, vs, vm, j, nm, ts);
    /**
     * @brief 减速段的参数计算
     */
    SCurveGetAccelerationParams(&sp->ap2, vm, ve, j, nm, ts);

    /* 计算加速段长和减速段长 */
    double la = SCurveAccelerationLength(&sp->ap1);
    double ld = SCurveAccelerationLength(&sp->ap2);
    FILE *fp = fopen("ap1_data.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "=== Acceleration Parameters (ap1) ===\n");
        fprintf(fp, "n1: %d\n", sp->ap1.n1);
        fprintf(fp, "n2: %d\n", sp->ap1.n2);
        fprintf(fp, "a: %f\n", sp->ap1.a);
        fprintf(fp, "j: %f\n", sp->ap1.j);
        fprintf(fp, "ts: %f\n", sp->ap1.ts);
        fprintf(fp, "v1: %f\n", sp->ap1.v1);
        fprintf(fp, "la: %f\n", la);
        fprintf(fp, "========================\n\n");
        fclose(fp);
    }
    fp = fopen("ap2_data.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "=== Acceleration Parameters (ap2) ===\n");
        fprintf(fp, "n1: %d\n", sp->ap2.n1);
        fprintf(fp, "n2: %d\n", sp->ap2.n2);
        fprintf(fp, "a: %f\n", sp->ap2.a);
        fprintf(fp, "j: %f\n", sp->ap2.j);
        fprintf(fp, "ts: %f\n", sp->ap2.ts);
        fprintf(fp, "v1: %f\n", sp->ap2.v1);
        fprintf(fp, "ld: %f\n", ld);
        fprintf(fp, "========================\n\n");
        fclose(fp);
    }

    /**
     * @brief 1 可以到达最大进给速度的情况。有四种情况
     *
     */
    if (l >= la + ld)
    {
        /* 计算匀速段长 */
        double ls = l - la - ld;

        /* 如果匀速段长过短？ */
        // TODO if (匀速段长过短) {}

        /* 情况 1：++ -+ 匀 +- -- */
        /* 情况 2：++ 匀+ -+ 匀 +- -- */
        /* 情况 3：++ -+ 匀 +- 匀- -- */
        /* 情况 4：++ 匀+ -+ 匀 +- 匀- -- */
        sp->Tx[0] = 1;
        sp->Tx[1] = sp->ap1.n1 + 1;
        sp->Tx[2] = sp->ap1.n1 + sp->ap1.n2 + 1;
        sp->Tx[3] = 2 * sp->ap1.n1 + sp->ap1.n2;

        /* 计算匀速段时间 */
        // double n3d = ls / (vm * sp->ap1.ts);
        int32_t n3 = ceil(ls / (vm * sp->ap1.ts));

        sp->Tx[4] = sp->Tx[3] + n3;

        sp->Tx[5] = sp->Tx[4] + 1;
        sp->Tx[6] = sp->Tx[4] + sp->ap2.n1 + 1;
        sp->Tx[7] = sp->Tx[4] + sp->ap2.n1 + sp->ap2.n2 + 1;
        sp->Tx[8] = sp->Tx[4] + 2 * sp->ap2.n1 + sp->ap2.n2;
    }
    else
    {
        /**
         * @brief 2 不能到达最大进给速度的情况，需要重新求取最大进给速度（二分法）
         */
        double vmax = vm;
        double vmin = (vs > ve ? vs : ve);

        /**
         * @brief 二分法求取新的 vm。
         */
        while (1)
        {
            double vmid = 0.5 * (vmin + vmax);

            double l1 = SCurveAccelerationLength(vs, vmid, j, nm, ts);
            double l2 = SCurveAccelerationLength(vmid, ve, j, nm, ts);
            double error = l1 + l2 - l;

            if (std::fabs(error) < 0.0001)
            {
                vm = vmid;
                break;
            }
            else if (error < 0)
            {
                vmin = vmid;
                vm = vmid;
            }
            else
            {
                vmax = vmid;
            }

            if (std::fabs(vmax - vmin) < 0.000001)
            {
                vm = vmin;
                break;
            }
        }

        // if (abs(vm-ve) < 0.001 && abs(vm-vs)<=0.001) {
        // /**
        //  * @brief 第一种情况：vs ≈ vm ≈ ve。采用匀速运动
        //  */
        //     /* 以 vs 进行匀速运动 */
        //     sp->Tx[0] = 0;
        //     sp->Tx[1] = 0;
        //     sp->Tx[2] = 0;
        //     sp->Tx[3] = 0;

        //     /* 计算匀速段时间 */
        //     int n3 = 0;
        //     n3 = ceil(l / (vs * sp->ap1.ts));

        //     sp->Tx[4] = sp->Tx[3] + n3;

        //     sp->Tx[5] = sp->Tx[4];
        //     sp->Tx[6] = sp->Tx[4];
        //     sp->Tx[7] = sp->Tx[4];
        //     sp->Tx[8] = sp->Tx[4];
        // } else {
        /**
         * @brief 第二种情况：vm >= vs, vm >= ve。采用 S 型加减速算法
         */
        /**
         * @brief 加速段的参数计算
         */
        SCurveGetAccelerationParams(&sp->ap1, vs, vm, j, nm, ts);

        /**
         * @brief 减速段的参数计算
         */
        SCurveGetAccelerationParams(&sp->ap2, vm, ve, j, nm, ts);

        /* 计算加速段长和减速段长 */
        la = SCurveAccelerationLength(&sp->ap1);
        ld = SCurveAccelerationLength(&sp->ap2);
        /* 计算匀速段长 */
        double ls = l - la - ld;

        sp->Tx[0] = 1;
        sp->Tx[1] = sp->ap1.n1 + 1;
        sp->Tx[2] = sp->ap1.n1 + sp->ap1.n2 + 1;
        sp->Tx[3] = 2 * sp->ap1.n1 + sp->ap1.n2;

        /* 计算匀速段时间 */
        int32_t n3 = 0;
        if (ls > 0)
        {
            n3 = ceil(ls / (vm * sp->ap1.ts));
        }

        sp->Tx[4] = sp->Tx[3] + n3;

        sp->Tx[5] = sp->Tx[4] + 1;
        sp->Tx[6] = sp->Tx[4] + sp->ap2.n1 + 1;
        sp->Tx[7] = sp->Tx[4] + sp->ap2.n1 + sp->ap2.n2 + 1;
        sp->Tx[8] = sp->Tx[4] + 2 * sp->ap2.n1 + sp->ap2.n2;
    }

    sp->si.vs = vs;
    sp->si.ve = ve;
    sp->si.vm = vm;
    sp->si.nm = nm;
    sp->si.l = l;
    sp->si.ts = ts;
    sp->ni = 1;
    sp->so.isOK = false;
    sp->so.sreal = 0;
    sp->so.last_sreal = 0;

    return 0;
}

bool SCurveExecute(SCurveParams *sp)
{
    int32_t ni = sp->ni;
    /* 插补已经结束，直接返回 0 */
    if (ni > sp->Tx[8] || sp->so.isOK)
    {
        sp->so.isOK = true;
        return true;
    }

    /* 插补周期 T */
    double t = sp->si.ts;

    /* 一 */
    if (ni <= sp->Tx[0])
    {
        sp->so.vreal = sp->si.vs;
    }
    /* 二 */
    else if (ni <= sp->Tx[1])
    {
        sp->so.vreal += (sp->ap1.a * t + (sp->ni - 2) * sp->ap1.j * pow(t, 2));
    }
    /* 三 */
    else if (ni <= sp->Tx[2])
    {
        sp->so.vreal += (sp->ap1.a * t + (sp->ap1.n1 - 1) * sp->ap1.j * pow(t, 2));
    }
    /* 四 */
    else if (ni <= sp->Tx[3])
    {
        sp->so.vreal += (sp->ap1.a * t + (2 * sp->ap1.n1 + sp->ap1.n2 - sp->ni) * sp->ap1.j * pow(t, 2));
    }
    /* 五 */
    else if (ni <= sp->Tx[4])
    {
        sp->so.vreal = sp->si.vm;
    }
    /* 六 */
    else if (ni <= sp->Tx[5])
    {
        sp->so.vreal = sp->si.vm;
    }
    /* 七 */
    else if (ni <= sp->Tx[6])
    {
        sp->so.vreal += (sp->ap2.a * t + (sp->ni - sp->Tx[4] - 2) * sp->ap2.j * pow(t, 2));
    }
    else if (ni <= sp->Tx[7])
    {
        sp->so.vreal += (sp->ap2.a * t + (sp->ap2.n1 - 1) * sp->ap2.j * pow(t, 2));
    }
    else if (ni <= sp->Tx[8])
    {
        sp->so.vreal += (sp->ap2.a * t + (2 * sp->ap2.n1 + sp->ap2.n2 - (sp->ni - sp->Tx[4])) * sp->ap2.j * pow(t, 2));
    }

    ++sp->ni;

    /* 如果要超过目标距离，需要约束并且结束当前插补 */
    double vreal = sp->so.vreal;
    if (sp->si.l - sp->so.sreal < sp->so.vreal * t)
    {
        vreal = (sp->si.l - sp->so.sreal) / t;
        sp->so.isOK = true;
    }
    /* 更新当前时刻的位置 */
    sp->so.vreal = vreal;
    /* 更新上一时刻位置 */
    sp->so.last_sreal = sp->so.sreal;
    /* 当前时刻的位置 */
    sp->so.sreal += vreal * t;

    /* 插补已经结束，直接返回 0 */
    if (sp->ni > sp->Tx[8])
    {
        sp->so.isOK = true;
    }

    return sp->so.isOK;
}
