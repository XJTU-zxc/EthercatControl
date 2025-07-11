#include "variabledef.h"
#include "params.h"
#include "stdlib.h"
// ---------------------------------------------------------------------------
// 函数功能：   反向间隙补偿
// 输入参数：
// 输出参数：
// 创建时间：   2006.09.14
// 最后修改时间:
// 修改原因：
// 创建人  ：   付科
// ---------------------------------------------------------------------------
union ParamUnion ParamterUnRegs;
struct RCCompStruct  RCCompRegs ;
void RCCompInit(void);
int RC(void);
void PC(void);
void myPC(void); //zjq 10.20


// 反向间隙
void RCCompInit(void)
{
    RCCompRegs.RC_x = 0;            // 反向间隙各轴状态标志
    RCCompRegs.RC_y = 0;
    RCCompRegs.RC_z = 0;
    RCCompRegs.RC_a = 0;
    RCCompRegs.RC_b = 0;
    RCCompRegs.RC_c = 0;
    RCCompRegs.RC_w = 0;
    RCCompRegs.RC_u = 0;
    RCCompRegs.RC_x_length = 20;     // 反向间隙长度
    RCCompRegs.RC_y_length = 40;
    RCCompRegs.RC_z_length = 60;
    RCCompRegs.RC_a_length = 20;
    RCCompRegs.RC_b_length = 40;
    RCCompRegs.RC_c_length = 60;
    RCCompRegs.RC_w_length = 80;
    RCCompRegs.RC_u_length = 100;
    RCCompRegs.RC_bak = PostionRegs.out_imp;
}
int RC(void)
{
    if(RCCompRegs.RC_bak.pos_x > 0 && PostionRegs.out_imp.pos_x < 0)                    // x由正到负
    {
        RCCompRegs.RC_x = -1;           // 正到负
        //RCCompRegs.RC_x_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[0];
    }
    else if(RCCompRegs.RC_bak.pos_x < 0 && PostionRegs.out_imp.pos_x > 0)           // x由负到正
    {
        RCCompRegs.RC_x = 1;            // 负到正
        //RCCompRegs.RC_x_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[0];
    }
    if(RCCompRegs.RC_bak.pos_y > 0 && PostionRegs.out_imp.pos_y < 0)                    // y由正到负
    {
        RCCompRegs.RC_y = -1;
    //  RCCompRegs.RC_y_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[1];
    }
    else if(RCCompRegs.RC_bak.pos_y < 0 && PostionRegs.out_imp.pos_y > 0)           // y由负到正
    {
        RCCompRegs.RC_y = 1;
    //  RCCompRegs.RC_y_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[1];
    }
    if(RCCompRegs.RC_bak.pos_z > 0 && PostionRegs.out_imp.pos_z < 0)                    // z由正到负
    {
        RCCompRegs.RC_z = -1;
    //  RCCompRegs.RC_z_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[2];
    }
    else if(RCCompRegs.RC_bak.pos_z < 0 && PostionRegs.out_imp.pos_z > 0)           // z由负到正
    {
        RCCompRegs.RC_z = 1;
    //  RCCompRegs.RC_z_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[2];
    }
    if(RCCompRegs.RC_bak.pos_a > 0 && PostionRegs.out_imp.pos_a < 0)                    // a由正到负
    {
        RCCompRegs.RC_a = -1;
    //  RCCompRegs.RC_a_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[3];
    }
    else if(RCCompRegs.RC_bak.pos_a < 0 && PostionRegs.out_imp.pos_a > 0)           // a由负到正
    {
        RCCompRegs.RC_a = 1;
    //  RCCompRegs.RC_a_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[3];
    }
    if(RCCompRegs.RC_bak.pos_b > 0 && PostionRegs.out_imp.pos_b < 0)                    // b由正到负
    {
        RCCompRegs.RC_b = -1;
    //  RCCompRegs.RC_b_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[4];
    }
    else if(RCCompRegs.RC_bak.pos_b < 0 && PostionRegs.out_imp.pos_b > 0)           // b由负到正
    {
        RCCompRegs.RC_b = 1;
    //  RCCompRegs.RC_b_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[4];
    }
    if(RCCompRegs.RC_bak.pos_c > 0 && PostionRegs.out_imp.pos_c < 0)                    // c由正到负
    {
        RCCompRegs.RC_c = -1;
    //  RCCompRegs.RC_c_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[5];
    }
    else if(RCCompRegs.RC_bak.pos_c < 0 && PostionRegs.out_imp.pos_c > 0)           // c由负到正
    {
        RCCompRegs.RC_c = 1;
    //  RCCompRegs.RC_c_length = ParamterUnRegs.ParamterRegs.AxisparmRegs.RCCompValue[5];
    }
    if(RCCompRegs.RC_bak.pos_w > 0 && PostionRegs.out_imp.pos_w < 0)
    {
        RCCompRegs.RC_w = -1;

    }
    else if(RCCompRegs.RC_bak.pos_w < 0 && PostionRegs.out_imp.pos_w > 0)
    {
        RCCompRegs.RC_w = 1;

    }
    if(RCCompRegs.RC_bak.pos_u > 0 && PostionRegs.out_imp.pos_u < 0)
    {
        RCCompRegs.RC_u = -1;

    }
    else if(RCCompRegs.RC_bak.pos_u < 0 && PostionRegs.out_imp.pos_u > 0)
    {
        RCCompRegs.RC_u = 1;

    }
    // 保存上次进给状态
    if(PostionRegs.out_imp.pos_x != 0)
        RCCompRegs.RC_bak.pos_x = PostionRegs.out_imp.pos_x;
    if(PostionRegs.out_imp.pos_y != 0)
        RCCompRegs.RC_bak.pos_y = PostionRegs.out_imp.pos_y;
    if(PostionRegs.out_imp.pos_z != 0)
        RCCompRegs.RC_bak.pos_z = PostionRegs.out_imp.pos_z;
    if(PostionRegs.out_imp.pos_a != 0)
        RCCompRegs.RC_bak.pos_a = PostionRegs.out_imp.pos_a;
    if(PostionRegs.out_imp.pos_b != 0)
        RCCompRegs.RC_bak.pos_b = PostionRegs.out_imp.pos_b;
    if(PostionRegs.out_imp.pos_c != 0)
        RCCompRegs.RC_bak.pos_c = PostionRegs.out_imp.pos_c;
    if(PostionRegs.out_imp.pos_w != 0)
        RCCompRegs.RC_bak.pos_w = PostionRegs.out_imp.pos_w;
    if(PostionRegs.out_imp.pos_u != 0)
        RCCompRegs.RC_bak.pos_u = PostionRegs.out_imp.pos_u;
    // 补偿开始
    if(RCCompRegs.RC_x == -1)
    {
        if(RCCompRegs.RC_x_length <= 3)
        {
            PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x - RCCompRegs.RC_x_length; // 补偿量小于3
            RCCompRegs.RC_x = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x - 3;              // 补偿量最大为3
            RCCompRegs.RC_x_length = RCCompRegs.RC_x_length - 3;
        }
    }

    if(RCCompRegs.RC_x == 1)
    {
        if(RCCompRegs.RC_x_length <= 3)
        {
            PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x + RCCompRegs.RC_x_length;
            RCCompRegs.RC_x = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x + 3;
            RCCompRegs.RC_x_length = RCCompRegs.RC_x_length - 3;
        }
    }

    if(RCCompRegs.RC_y == -1)
    {
        if(RCCompRegs.RC_y_length <= 3)
        {
            PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y - RCCompRegs.RC_y_length;
            RCCompRegs.RC_y = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y - 3;
            RCCompRegs.RC_y_length = RCCompRegs.RC_y_length - 3;
        }
    }

    if(RCCompRegs.RC_y == 1)
    {
        if(RCCompRegs.RC_y_length <= 3)
        {
            PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y + RCCompRegs.RC_y_length;
            RCCompRegs.RC_y = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y + 3;
            RCCompRegs.RC_y_length = RCCompRegs.RC_y_length - 3;
        }
    }

    if(RCCompRegs.RC_z == -1)
    {
        if(RCCompRegs.RC_z_length <= 3)
        {
            PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z - RCCompRegs.RC_z_length;
            RCCompRegs.RC_z = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z - 3;
            RCCompRegs.RC_z_length = RCCompRegs.RC_z_length - 3;
        }
    }

    if(RCCompRegs.RC_z == 1)
    {
        if(RCCompRegs.RC_z_length <= 3)
        {
            PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z + RCCompRegs.RC_z_length;
            RCCompRegs.RC_z = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z + 3;
            RCCompRegs.RC_z_length = RCCompRegs.RC_z_length - 3;
        }
    }

    if(RCCompRegs.RC_a == -1)
    {
        if(RCCompRegs.RC_a_length <= 3)
        {
            PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a - RCCompRegs.RC_a_length;
            RCCompRegs.RC_a = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a - 3;
            RCCompRegs.RC_a_length = RCCompRegs.RC_a_length - 3;
        }
    }

    if(RCCompRegs.RC_a == 1)
    {
        if(RCCompRegs.RC_a_length <= 3)
        {
            PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a + RCCompRegs.RC_a_length;
            RCCompRegs.RC_a = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a + 3;
            RCCompRegs.RC_a_length = RCCompRegs.RC_a_length - 3;
        }
    }

    if(RCCompRegs.RC_b == -1)
    {
        if(RCCompRegs.RC_b_length <= 3)
        {
            PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b - RCCompRegs.RC_b_length;
            RCCompRegs.RC_b = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b - 3;
            RCCompRegs.RC_b_length = RCCompRegs.RC_b_length - 3;
        }
    }

    if(RCCompRegs.RC_b == 1)
    {
        if(RCCompRegs.RC_b_length <= 3)
        {
            PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b + RCCompRegs.RC_b_length;
            RCCompRegs.RC_b = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b + 3;
            RCCompRegs.RC_b_length = RCCompRegs.RC_b_length - 3;
        }
    }

    if(RCCompRegs.RC_c == -1)
    {
        if(RCCompRegs.RC_c_length <= 3)
        {
            PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c - RCCompRegs.RC_c_length;
            RCCompRegs.RC_c = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c - 3;
            RCCompRegs.RC_c_length = RCCompRegs.RC_c_length - 3;
        }
    }

    if(RCCompRegs.RC_c == 1)
    {
        if(RCCompRegs.RC_c_length <= 3)
        {
            PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c + RCCompRegs.RC_c_length;
            RCCompRegs.RC_c = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c + 3;
            RCCompRegs.RC_c_length = RCCompRegs.RC_c_length - 3;
        }
    }

    if(RCCompRegs.RC_w == -1)
        {
            if(RCCompRegs.RC_w_length <= 3)
            {
                PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w - RCCompRegs.RC_w_length;
                RCCompRegs.RC_w = 0;
            }
            else
            {
                PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w - 3;
                RCCompRegs.RC_w_length = RCCompRegs.RC_w_length - 3;
            }
        }

    if(RCCompRegs.RC_w == 1)
    {
        if(RCCompRegs.RC_w_length <= 3)
        {
            PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w + RCCompRegs.RC_w_length;
            RCCompRegs.RC_w = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w + 3;
            RCCompRegs.RC_w_length = RCCompRegs.RC_w_length - 3;
        }
    }

    if(RCCompRegs.RC_u == -1)
        {
            if(RCCompRegs.RC_u_length <= 3)
            {
                PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u - RCCompRegs.RC_u_length;
                RCCompRegs.RC_u = 0;
            }
            else
            {
                PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u - 3;
                RCCompRegs.RC_u_length = RCCompRegs.RC_u_length - 3;
            }
        }

    if(RCCompRegs.RC_u == 1)
    {
        if(RCCompRegs.RC_u_length <= 3)
        {
            PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u + RCCompRegs.RC_u_length;
            RCCompRegs.RC_u = 0;
        }
        else
        {
            PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u + 3;
            RCCompRegs.RC_u_length = RCCompRegs.RC_u_length - 3;
        }
    }

    if(RCCompRegs.RC_x == 0 && RCCompRegs.RC_y == 0 && RCCompRegs.RC_z == 0 && RCCompRegs.RC_a == 0 && RCCompRegs.RC_b == 0 && RCCompRegs.RC_c == 0  && RCCompRegs.RC_w == 0&& RCCompRegs.RC_u == 0)
        return 0;           // 反向补偿完毕
    else
        return 1;           // 反向补偿未完成
}
// ---------------------------------------------------------------------------
// 函数功能：   螺距补偿
// 输入参数：
// 输出参数：
// 创建时间：   2006.09.16
// 最后修改时间:
// 修改原因：
// 创建人  ：   付科
// ---------------------------------------------------------------------------
void PCCompInit(void)
{

}
void PC(void)
{
    int temp1, temp2;
    int32_t temp32;
// x轴计算
    if (ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[0])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[0];//螺补机械零点
        temp32 =1;// PostionRegs.real_posbk.pos_x / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[0];
        temp1 =(int)temp32;
        if(PostionRegs.real_posbk.pos_x < 0)
            temp1 = temp1 - 1;
        //temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[0] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[0] / 2;
        temp1 = temp1 + temp2;  // 计算当前位置的补偿段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pcxr_add = &ParamterUnRegs.ParamterRegs.PCCommpX[0];
            if(PostionRegs.out_imp.pos_x > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_x < temp32 && PostionRegs.real_pos.pos_x > temp32)// 正向跨界判断
                {
                    if(*(pcxr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x + *(pcxr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_x == temp32)
                {
                    if(*(pcxr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x + *(pcxr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_x == temp32)
                {
                    if(*(pcxr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x + *(pcxr_add + temp1) - *(pcxr_add + temp1) / 2;
                }
            }
            else if(PostionRegs.out_imp.pos_x < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_x > temp32 && PostionRegs.real_pos.pos_x < temp32)// 负向跨界判断
                {
                    if(*(pcxr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x - *(pcxr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_x == temp32)
                {
                    if(*(pcxr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x - *(pcxr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_x == temp32)
                {
                    if(*(pcxr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_x = PostionRegs.out_imp.pos_x - *(pcxr_add + temp1) + *(pcxr_add + temp1) / 2;
                }
            }
        }
    }

// y
    if (ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[1])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[1];
    //  temp32 = PostionRegs.real_posbk.pos_y / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[1];
        temp1 = (int)temp32;
        if(PostionRegs.real_posbk.pos_y < 0)
            temp1 = temp1 - 1;
    //  temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[1] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[1] / 2;
        temp1 = temp1 + temp2;  // 当前段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pcyr_add = &ParamterUnRegs.ParamterRegs.PCCommpY[0];
            if(PostionRegs.out_imp.pos_y > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_y < temp32 && PostionRegs.real_pos.pos_y > temp32)// 正向跨界判断
                {
                    if(*(pcyr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y + *(pcyr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_y == temp32)
                {
                    if(*(pcyr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y + *(pcyr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_y == temp32)
                {
                    if(*(pcyr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y + *(pcyr_add + temp1) - *(pcyr_add + temp1) / 2;
                }
            }
            else if(PostionRegs.out_imp.pos_y < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_y > temp32 && PostionRegs.real_pos.pos_y < temp32)// 负向跨界判断
                {
                    if(*(pcyr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y - *(pcyr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_y == temp32)
                {
                    if(*(pcyr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y - *(pcyr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_y == temp32)
                {
                    if(*(pcyr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_y = PostionRegs.out_imp.pos_y - *(pcyr_add + temp1) + *(pcyr_add + temp1) / 2;
                }
            }
        }
    }
// z
    if(ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[2])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[2];
    //  temp32 = PostionRegs.real_posbk.pos_z / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[2];
        temp1 = (int)temp32;
        if(PostionRegs.real_posbk.pos_z < 0)
            temp1 = temp1 - 1;
    //  temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[2] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[2] / 2;
        temp1 = temp1 + temp2;  // 当前段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pczr_add = &ParamterUnRegs.ParamterRegs.PCCommpZ[0];
            if(PostionRegs.out_imp.pos_z > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_z < temp32 && PostionRegs.real_pos.pos_z > temp32)// 正向跨界判断
                {
                    if(*(pczr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z + *(pczr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_z == temp32)
                {
                    if(*(pczr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z + *(pczr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_z == temp32)
                {
                    if(*(pczr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z + *(pczr_add + temp1) - *(pczr_add + temp1)/ 2;
                }
            }
            else if(PostionRegs.out_imp.pos_z < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_z > temp32 && PostionRegs.real_pos.pos_z < temp32)// 负向跨界判断
                {
                    if(*(pczr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z - *(pczr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_z == temp32)
                {
                    if(*(pczr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z - *(pczr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_z == temp32)
                {
                    if(*(pczr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_z = PostionRegs.out_imp.pos_z - *(pczr_add + temp1) + *(pczr_add + temp1) / 2;
                }
            }
        }
    }
// a
    if(ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[3])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[3];
    //  temp32 = PostionRegs.real_posbk.pos_a / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[3];
        temp1 = (int)temp32;
        if(PostionRegs.real_posbk.pos_a < 0)
            temp1 = temp1 - 1;
    //  temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[3] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[3] / 2;
        temp1 = temp1 + temp2;  // 当前段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pcar_add = &ParamterUnRegs.ParamterRegs.PCCommpA[0];
            if(PostionRegs.out_imp.pos_a > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_a < temp32 && PostionRegs.real_pos.pos_a > temp32)// 正向跨界判断
                {
                    if(*(pcar_add + temp1) != 0)
                        PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a + *(pcar_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_a == temp32)
                {
                    if(*(pcar_add + temp1) != 0)
                        PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a + *(pcar_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_a == temp32)
                {
                    if(*(pcar_add + temp1) != 0)
                        PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a + *(pcar_add + temp1) - *(pcar_add + temp1) / 2;
                }
            }
            else if(PostionRegs.out_imp.pos_a < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_a > temp32 && PostionRegs.real_pos.pos_a < temp32)// 负向跨界判断
                {
                    if(*(pcar_add + temp1) != 0)
                        PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a - *(pcar_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_a == temp32)
                {
                    if(*(pcar_add + temp1) != 0)
                        PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a - *(pcar_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_a == temp32)
                {
                    if(*(pcar_add + temp1) != 0)
                        PostionRegs.out_imp.pos_a = PostionRegs.out_imp.pos_a - *(pcar_add + temp1) + *(pcar_add + temp1) / 2;
                }
            }
        }
    }
// b
    if(ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[4])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[4];
    //  temp32 = PostionRegs.real_posbk.pos_b / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[4];
        temp1 = (int)temp32;
        if(PostionRegs.real_posbk.pos_b < 0)
            temp1 = temp1 - 1;
    //  temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[4] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[4] / 2;
        temp1 = temp1 + temp2;  // 当前段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pcbr_add = &ParamterUnRegs.ParamterRegs.PCCommpB[0];
            if(PostionRegs.out_imp.pos_b > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_b < temp32 && PostionRegs.real_pos.pos_b > temp32)// 正向跨界判断
                {
                    if(*(pcbr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b + *(pcbr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_b == temp32)
                {
                    if(*(pcbr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b + *(pcbr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_b == temp32)
                {
                    if(*(pcbr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b + *(pcbr_add + temp1) - *(pcbr_add + temp1) / 2;
                }
            }
            else if(PostionRegs.out_imp.pos_b < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_b > temp32 && PostionRegs.real_pos.pos_b < temp32)// 负向跨界判断
                {
                    if(*(pcbr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b - *(pcbr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_b == temp32)
                {
                    if(*(pcbr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b - *(pcbr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_b == temp32)
                {
                    if(*(pcbr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_b = PostionRegs.out_imp.pos_b - *(pcbr_add + temp1) + *(pcbr_add + temp1) / 2;
                }
            }
        }
    }
// c
    if(ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[5];
    //  temp32 = PostionRegs.real_posbk.pos_c / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5];
        temp1 = (int)temp32;
        if(PostionRegs.real_posbk.pos_c < 0)
            temp1 = temp1 - 1;
    //  temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5] / 2;
        temp1 = temp1 + temp2;  // 当前段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pccr_add = &ParamterUnRegs.ParamterRegs.PCCommpC[0];
            if(PostionRegs.out_imp.pos_c > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_c < temp32 && PostionRegs.real_pos.pos_c > temp32)// 正向跨界判断
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c + *(pccr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_c == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c + *(pccr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_c == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c + *(pccr_add + temp1) - *(pccr_add + temp1) / 2;
                }
            }
            else if(PostionRegs.out_imp.pos_c < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_c > temp32 && PostionRegs.real_pos.pos_c < temp32)// 负向跨界判断
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c - *(pccr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_c == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c - *(pccr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_c == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_c = PostionRegs.out_imp.pos_c - *(pccr_add + temp1) + *(pccr_add + temp1) / 2;
                }
            }
        }
    }

    if(ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[6])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[6];
    //  temp32 = PostionRegs.real_posbk.pos_c / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5];
        temp1 = (int)temp32;
        if(PostionRegs.real_posbk.pos_w < 0)
            temp1 = temp1 - 1;
    //  temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5] / 2;
        temp1 = temp1 + temp2;  // 当前段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pccr_add = &ParamterUnRegs.ParamterRegs.PCCommpC[0];
            if(PostionRegs.out_imp.pos_w > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_w < temp32 && PostionRegs.real_pos.pos_w > temp32)// 正向跨界判断
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w + *(pccr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_w == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w + *(pccr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_w == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w + *(pccr_add + temp1) - *(pccr_add + temp1) / 2;
                }
            }
            else if(PostionRegs.out_imp.pos_w < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_w > temp32 && PostionRegs.real_pos.pos_w < temp32)// 负向跨界判断
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w - *(pccr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_w == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w - *(pccr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_w == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_w = PostionRegs.out_imp.pos_w - *(pccr_add + temp1) + *(pccr_add + temp1) / 2;
                }
            }
        }
    }

    if(ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[7])
    {
        temp2 = ParamterUnRegs.ParamterRegs.m_commonparmRegs.PCZeroPoint[5];
    //  temp32 = PostionRegs.real_posbk.pos_c / ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5];
        temp1 = (int)temp32;
        if(PostionRegs.real_posbk.pos_u< 0)
            temp1 = temp1 - 1;
    //  temp32 = temp1 * ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5] + ParamterUnRegs.ParamterRegs.AxisparmRegs.PCInt[5] / 2;
        temp1 = temp1 + temp2;  // 当前段数
        if(temp1 <= 127 && temp1 >= 0)
        {
            int16_t *pccr_add = &ParamterUnRegs.ParamterRegs.PCCommpC[0];
            if(PostionRegs.out_imp.pos_u > 0)   // 正向镙补
            {
                if(PostionRegs.real_posbk.pos_u < temp32 && PostionRegs.real_pos.pos_u > temp32)// 正向跨界判断
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u + *(pccr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_u == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u + *(pccr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_u == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u + *(pccr_add + temp1) - *(pccr_add + temp1) / 2;
                }
            }
            else if(PostionRegs.out_imp.pos_u < 0)              // 负向镙补
            {
                if(PostionRegs.real_posbk.pos_u > temp32 && PostionRegs.real_pos.pos_u < temp32)// 负向跨界判断
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u - *(pccr_add + temp1);
                }
                else if(PostionRegs.real_pos.pos_u == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u - *(pccr_add + temp1) / 2;
                }
                else if(PostionRegs.real_posbk.pos_u == temp32)
                {
                    if(*(pccr_add + temp1) != 0)
                        PostionRegs.out_imp.pos_u = PostionRegs.out_imp.pos_u - *(pccr_add + temp1) + *(pccr_add + temp1) / 2;
                }
            }
        }
    }
}


/**
 *
 * 程序作用：螺补程序
 * zjq 10.20
 *
 * **/
void myPC(void)
{
    //x
    if (PostionRegs.real_pos.pos_x >= PostionRegs.real_posbk.pos_x) {  //x轴正向螺补
        if ( (PostionRegs.real_pos.pos_x - g_dzn_len[0]) / g_dit_param != (PostionRegs.real_posbk.pos_x - g_dzn_len[0]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_x - g_dzn_len[0]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.real_posbk.pos_x + g_luobu_arr[(PostionRegs.real_pos.pos_x - g_dzn_len[0]) / g_dit_param];
        }
    }else {   //x轴负向螺补
        if ( (PostionRegs.real_pos.pos_x - g_dzn_len[0]) / g_dit_param != (PostionRegs.real_posbk.pos_x - g_dzn_len[0]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_x - g_dzn_len[0]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.real_posbk.pos_x + g_luobu_arr[abs(PostionRegs.real_pos.pos_x - g_dzn_len[0]) / g_dit_param];
        }
    }

    //y
    if (PostionRegs.real_pos.pos_y >= PostionRegs.real_posbk.pos_y) {  //y轴正向螺补
        if ( (PostionRegs.real_pos.pos_y - g_dzn_len[1]) / g_dit_param != (PostionRegs.real_posbk.pos_y - g_dzn_len[1]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_y - g_dzn_len[1]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.real_posbk.pos_y + g_luobu_arr[(PostionRegs.real_pos.pos_y - g_dzn_len[1]) / g_dit_param];
        }
    }else {   //y轴负向螺补
        if ( (PostionRegs.real_pos.pos_y - g_dzn_len[1]) / g_dit_param != (PostionRegs.real_posbk.pos_y - g_dzn_len[1]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_y - g_dzn_len[1]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.real_posbk.pos_y + g_luobu_arr[abs(PostionRegs.real_pos.pos_y - g_dzn_len[1]) / g_dit_param];
        }
    }

    //z
    if (PostionRegs.real_pos.pos_z >= PostionRegs.real_posbk.pos_z) {  //z轴正向螺补
        if ( (PostionRegs.real_pos.pos_z - g_dzn_len[2]) / g_dit_param != (PostionRegs.real_posbk.pos_z - g_dzn_len[2]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_z - g_dzn_len[2]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.real_posbk.pos_z + g_luobu_arr[(PostionRegs.real_pos.pos_z - g_dzn_len[2]) / g_dit_param];
        }
    }else {   //z轴负向螺补
        if ( (PostionRegs.real_pos.pos_z - g_dzn_len[2]) / g_dit_param != (PostionRegs.real_posbk.pos_z - g_dzn_len[2]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_z - g_dzn_len[2]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.real_posbk.pos_z + g_luobu_arr[abs(PostionRegs.real_pos.pos_z - g_dzn_len[2]) / g_dit_param];
        }
    }

    //a
    if (PostionRegs.real_pos.pos_a >= PostionRegs.real_posbk.pos_a) {  //a轴正向螺补
        if ( (PostionRegs.real_pos.pos_a - g_dzn_len[3]) / g_dit_param != (PostionRegs.real_posbk.pos_a - g_dzn_len[3]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_a - g_dzn_len[3]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.real_posbk.pos_a + g_luobu_arr[(PostionRegs.real_pos.pos_a - g_dzn_len[3]) / g_dit_param];
        }
    }else {   //a轴负向螺补
        if ( (PostionRegs.real_pos.pos_a - g_dzn_len[3]) / g_dit_param != (PostionRegs.real_posbk.pos_a - g_dzn_len[3]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_a - g_dzn_len[3]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.real_posbk.pos_a + g_luobu_arr[abs(PostionRegs.real_pos.pos_a - g_dzn_len[3]) / g_dit_param];
        }
    }


    //b
    if (PostionRegs.real_pos.pos_b >= PostionRegs.real_posbk.pos_b) {  //b轴正向螺补
        if ( (PostionRegs.real_pos.pos_b - g_dzn_len[4]) / g_dit_param != (PostionRegs.real_posbk.pos_b - g_dzn_len[4]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_b - g_dzn_len[4]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.real_posbk.pos_b + g_luobu_arr[(PostionRegs.real_pos.pos_b - g_dzn_len[4]) / g_dit_param];
        }
    }else {   //b轴负向螺补
        if ( (PostionRegs.real_pos.pos_b - g_dzn_len[4]) / g_dit_param != (PostionRegs.real_posbk.pos_b - g_dzn_len[4]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_b - g_dzn_len[4]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.real_posbk.pos_b + g_luobu_arr[abs(PostionRegs.real_pos.pos_b - g_dzn_len[4]) / g_dit_param];
        }
    }


    //c
    if (PostionRegs.real_pos.pos_c >= PostionRegs.real_posbk.pos_c) {  //c轴正向螺补
        if ( (PostionRegs.real_pos.pos_c - g_dzn_len[5]) / g_dit_param != (PostionRegs.real_posbk.pos_c - g_dzn_len[5]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_c - g_dzn_len[5]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.real_posbk.pos_c + g_luobu_arr[(PostionRegs.real_pos.pos_c - g_dzn_len[5]) / g_dit_param];
        }
    }else {   //c轴负向螺补
        if ( (PostionRegs.real_pos.pos_c - g_dzn_len[5]) / g_dit_param != (PostionRegs.real_posbk.pos_c - g_dzn_len[5]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_c - g_dzn_len[5]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.real_posbk.pos_c + g_luobu_arr[abs(PostionRegs.real_pos.pos_c - g_dzn_len[5]) / g_dit_param];
        }
    }

    //w
    if (PostionRegs.real_pos.pos_w >= PostionRegs.real_posbk.pos_w) {  //w轴正向螺补
        if ( (PostionRegs.real_pos.pos_w - g_dzn_len[6]) / g_dit_param != (PostionRegs.real_posbk.pos_w - g_dzn_len[6]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_w - g_dzn_len[6]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.real_posbk.pos_w + g_luobu_arr[(PostionRegs.real_pos.pos_w - g_dzn_len[6]) / g_dit_param];
        }
    }else {   //w轴负向螺补
        if ( (PostionRegs.real_pos.pos_w - g_dzn_len[6]) / g_dit_param != (PostionRegs.real_posbk.pos_w - g_dzn_len[6]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_w - g_dzn_len[6]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.real_posbk.pos_w + g_luobu_arr[abs(PostionRegs.real_pos.pos_w - g_dzn_len[6]) / g_dit_param];
        }
    }


    //u
    if (PostionRegs.real_pos.pos_u >= PostionRegs.real_posbk.pos_u) {  //u轴正向螺补
        if ( (PostionRegs.real_pos.pos_u - g_dzn_len[7]) / g_dit_param != (PostionRegs.real_posbk.pos_u - g_dzn_len[7]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if ((PostionRegs.real_pos.pos_u - g_dzn_len[7]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.real_posbk.pos_u + g_luobu_arr[(PostionRegs.real_pos.pos_u - g_dzn_len[7]) / g_dit_param];
        }
    }else {   //u轴负向螺补
        if ( (PostionRegs.real_pos.pos_u - g_dzn_len[7]) / g_dit_param != (PostionRegs.real_posbk.pos_u - g_dzn_len[7]) / g_dit_param ) { //本次计算的脉冲数距离零点脉冲数的倍数不等于上次的，说明需要进行补偿
            if (abs(PostionRegs.real_pos.pos_u - g_dzn_len[7]) / g_dit_param < G_LUOBU_ARR_LEN)
                PostionRegs.out_imp.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.real_posbk.pos_u + g_luobu_arr[abs(PostionRegs.real_pos.pos_u - g_dzn_len[7]) / g_dit_param];
        }
    }

}