
#include "variabledef.h"
#include "math.h"
#include "stdlib.h"
struct  inter_pos V_max;       // 各轴最大速度
extern unsigned short In_PLC2CNC_D;
struct AccDecVarStruct SpeedRegs;
struct EncodeSTReg EncodeRegs;
struct MainAxisST  MainAxisRegs;
/**********Speed Add*/////////////////
int32_t S_remnant_up = 0; //chenlin2014.6.20
int32_t S_remnant_down = 0;//chenlin2014.6.20
int32_t spd_down_step2_flag ;//chenlin2014.5.27
int32_t ud_length1_true;//chenlin2014.5.27
struct JOGSTREGS JOGRegs;
/**********Speed Add*/////////////////
////S曲线速度控制方法初始化//
// 最大合成加速度Amax= sqrt(axmax *  axmax +  aymax *  aymax + azmax *  azmax +...)
// 系统加加速度 = Amax / tm
// 加减速初始化模块speed_init（）
// 功能描述： ， ， ， 变量赋初值，最大合成加速度 及加加速度 的计算
// 输入变量：  ， ， ， ，
// 输出变量：  ， ， ， ，
// 算法描述：
// （a）变量赋初值： ， ， ，
// （b）常数计算： ，
void Speed_init(int32_t Tm, int32_t ax, int32_t ay, int32_t az, int32_t aa, int32_t ab, int32_t ac)
{
    struct INT64 temp1,temp2;
    temp1.int_l=0;
    temp1.int_h=0;
    temp2.int_l=0;
    temp2.int_h=0;
    SpeedRegs.tm= Tm;               // 加速度从0到最大合成加速度 （或从 减至0）的时间
    SpeedRegs.Ac.pos_x= ax;         // 接受传参
    SpeedRegs.Ac.pos_y= ay;
    SpeedRegs.Ac.pos_z= az;
    SpeedRegs.Ac.pos_a= aa;
    SpeedRegs.Ac.pos_b= ab;
    SpeedRegs.Ac.pos_c= ac;
    temp1=mul64(SpeedRegs.Ac.pos_x,SpeedRegs.Ac.pos_x);   // 求解Amax
    temp2=mul64(SpeedRegs.Ac.pos_y,SpeedRegs.Ac.pos_y);
    temp1=add64(temp1,temp2);
    temp2=mul64(SpeedRegs.Ac.pos_z,SpeedRegs.Ac.pos_z);
    temp1=add64(temp1,temp2);
    temp2=mul64(SpeedRegs.Ac.pos_a,SpeedRegs.Ac.pos_a);
    temp1=add64(temp1,temp2);
    temp2=mul64(SpeedRegs.Ac.pos_b,SpeedRegs.Ac.pos_b);
    temp1=add64(temp1,temp2);
    temp2=mul64(SpeedRegs.Ac.pos_c,SpeedRegs.Ac.pos_c);
    temp1=add64(temp1,temp2);
    SpeedRegs.Amax=sqr64(temp1);                       // 最大合成加速度
    SpeedRegs.JJSpeed=(double)SpeedRegs.Amax / SpeedRegs.tm+0.5; // 系统加加速度
//    SpeedRegs.JJSpeed = 0.1;
    SpeedRegs.Nmax= SpeedRegs.tm;                      //
    SpeedRegs.maxEr=5;                                //  二分法求 和 的迭代终止条件中的允许误差
}
////计算本插补周期最大加速度以及达到最大加速度所需要的插补周期数//
// 最大允许加速度acc（）
// 功能描述：计算程序段 的最大允许加速度 ，加速度由0 达到最大值 （或 减至0）的插补周期数
// 输入变量： 在起点处的单位方向矢量 ，
// 在终点处的单位方向矢量 ，
//         各轴最大进给加速度 ， ， ，系统加加速度 ，插补周期
// 输出变量：第 段最大允许加速度 ，加速度由0 达到最大值 （或 减至0）的插补周期数
// 内部变量：无
int32_t NmiCal(struct inter_pos eSt,struct inter_pos eEnd,int32_t L1,int32_t L2)
{
    int32_t temp1,temp2;
    temp1=9999;
    if(eSt.pos_x!=0)
    {
        eSt.pos_x = (SpeedRegs.Ac.pos_x*L1)/abs(eSt.pos_x);
        temp1 = eSt.pos_x;
    }
    if(eSt.pos_y!=0)
    {
        eSt.pos_y = (SpeedRegs.Ac.pos_y*L1)/abs(eSt.pos_y);
        if(temp1 > eSt.pos_y)
            temp1 = eSt.pos_y;
    }
    if(eSt.pos_z!=0)
    {
        eSt.pos_z = (SpeedRegs.Ac.pos_z*L1)/labs(eSt.pos_z);
        if(temp1 > eSt.pos_z)
            temp1 = eSt.pos_z;
    }
    if(eSt.pos_a!=0)
    {
        eSt.pos_a = (SpeedRegs.Ac.pos_a*L1)/labs(eSt.pos_a);
        if(temp1 > eSt.pos_a)
            temp1 = eSt.pos_a;
    }
    if(eSt.pos_b!=0)
    {
        eSt.pos_b = (SpeedRegs.Ac.pos_b*L1)/labs(eSt.pos_b);
        if(temp1 > eSt.pos_b)
            temp1 = eSt.pos_b;
    }
    if(eSt.pos_c!=0)
    {
        eSt.pos_c = (SpeedRegs.Ac.pos_b*L1)/labs(eSt.pos_c);
        if(temp1 > eSt.pos_c)
            temp1 = eSt.pos_c;
    }
    temp2 = (float)temp1 / SpeedRegs.JJSpeed;
    return temp2 ;
}
// ---------------------------------------------------------------------------
// 函数功能：   S曲线加速段或减速段规划
// 输入参数：   起点速度、终点速度、本段Nmi
// 输出参数：   加速段段长或减速段段长
// 创建时间：   2010
// ---------------------------------------------------------------------------
int32_t Lim_speed(int32_t vs, int32_t ve, int32_t Nmi)
{
    int32_t Nm;
    int32_t M1, M2, n1, n2, J1;
    int32_t ap1;
    int32_t vs_temp;
    float tempfloat1;
    float tempfloat2,tempfloat3;
    if(vs >= (ve - 2*SpeedRegs.Epslion) && vs <= (ve + 2*SpeedRegs.Epslion))            // v0 = ve,无加减速
    {
        SpeedRegs.length=0;
        return SpeedRegs.length;
    }
    else
    {
        SpeedRegs.length=0;
        Nm = Nmi;
        tempfloat1 = (labs(vs-ve));
        tempfloat2 = ((Nm*SpeedRegs.JJSpeed));
        tempfloat1 = tempfloat1/tempfloat2;
        tempfloat2 = (Nm+1);
        tempfloat1 =tempfloat1 - tempfloat2;
        tempfloat2 = floor(tempfloat1);//Xia qu zheng
        if(tempfloat1 != tempfloat2)//shang qu zheng
            M1 = (int32_t)tempfloat2 + 1;
        else
            M1 = (int32_t)tempfloat1;
        tempfloat1 = (4*labs(vs-ve));
        tempfloat2 = SpeedRegs.JJSpeed;
        tempfloat1 = tempfloat1 / tempfloat2;
        tempfloat1 = tempfloat1+1;
        tempfloat1 = sqrt(tempfloat1);
        tempfloat1 = (tempfloat1 - 1) / 2;
        tempfloat2 = floor(tempfloat1 + 0.00001);//Xia qu zheng//chenlin2014.5.29
        M2 = tempfloat2;//chenlin2014.5.29
        if(M1 > 0)
        {
            n1 = Nm;
            n2 = M1;
        }
        else
        {
            n1 = M2;
            n2 = 0;
        }
        tempfloat1 = (float)labs(vs-ve) - (float)(n1+n2)*(n1-1)*SpeedRegs.JJSpeed;
        tempfloat2 = (2*n1+n2);
        if(vs < ve)
        {
            ap1 = tempfloat1 / tempfloat2-0.5;
            //ap1 = tempfloat1 / tempfloat2;//chenlin2014.6.21shanchu
            SpeedRegs.a_ini_up = ap1 = 1;//chenlin2014.6.21
        //  SpeedRegs.a_ini_up = ap1 + 0.99;//chenlin2014.5.29//chenlin2014.6.21shanchu
        //  SpeedRegs.a_ini_up = ap1;//chenlin2014.5.29beifen
            J1 = SpeedRegs.JJSpeed;
            SpeedRegs.spd_up_step2=n1;
        //  n1 = n1 - 1;
            SpeedRegs.spd_up_step3=n2;
            SpeedRegs.spd_up_step=2*n1+1+n2;
            tempfloat1 = (n1+1)*vs+n1*(n1+1)*(n1+2)*J1/6;
            tempfloat2 = n1*vs+0.5*n1*n1*(n1+2*n2+1)*J1 +n1*(n1+1)*(2*n1+1)*J1/6;
            tempfloat3 = 0;
        }
        else if(vs > ve)
        {
            ap1 = tempfloat1 / tempfloat2 ;//chenlin2014.6.11改之前没有-0.1
            ap1 = 1;
            SpeedRegs.a_ini_down = -ap1 ;
            SpeedRegs.a_ini_down = -1;//chenlin2014.5.27
            J1 = -SpeedRegs.JJSpeed;
            SpeedRegs.spd_down_step2=n1;
            SpeedRegs.spd_down_step3=n2;
            SpeedRegs.spd_down_step=2*n1+1+n2;
            if(vs == SpeedRegs.spd_down_step2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step2+4 && (vs == SpeedRegs.speed_min ||ve == SpeedRegs.speed_min))
            {
                tempfloat1 = (2*n1+n2+1)*vs;
                tempfloat2 = -0.5*ap1*(4*n1*n1+2*n1+4*n1*n2+n2*n2+n2);
                tempfloat3 = 0.5*(2*n1*n1*n1+3*n1*n1*n2-n1*n1+n1*n2*n2-2*n1*n2-n1-n2*n2-n2)*J1;
                vs_temp = vs;//chenlin2014.7.1
            }
            else if(vs == SpeedRegs.speed_min ||ve == SpeedRegs.speed_min)
            {
                vs_temp = vs;
                vs = SpeedRegs.spd_down_step2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step2+4;
                tempfloat1 = (2*n1+n2+1)*vs;
                tempfloat2 = -0.5*ap1*(4*n1*n1+2*n1+4*n1*n2+n2*n2+n2);
                tempfloat3 = 0.5*(2*n1*n1*n1+3*n1*n1*n2-n1*n1+n1*n2*n2-2*n1*n2-n1-n2*n2-n2)*J1;
            }
            else
            {
                tempfloat1 = (2*n1+n2+1)*vs;
                tempfloat2 = -0.5*ap1*(4*n1*n1+2*n1+4*n1*n2+n2*n2+n2);
                tempfloat3 = 0.5*(2*n1*n1*n1+3*n1*n1*n2-n1*n1+n1*n2*n2-2*n1*n2-n1-n2*n2-n2)*J1;

            }
        }
        SpeedRegs.length = (int32_t)(tempfloat1+tempfloat2+tempfloat3);
        if( vs < ve && (vs == SpeedRegs.speed_min ||ve == SpeedRegs.speed_min))
        {
            if(ve != SpeedRegs.spd_up_step2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step2+4)
            {
                S_remnant_up = SpeedRegs.sect_in.feed - (SpeedRegs.spd_up_step2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step2+4);
            //  length = length + 1 + S_remnant_up*n1*2;
                SpeedRegs.length = SpeedRegs.length + 1 + 0.5*S_remnant_up-0.5*S_remnant_up*S_remnant_up+S_remnant_up*SpeedRegs.spd_up_step;
            }

        }
        if( ve < vs && (vs == SpeedRegs.speed_min ||ve == SpeedRegs.speed_min))
        {
            if(vs_temp != SpeedRegs.spd_down_step2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step2+4)
            {
                S_remnant_down = SpeedRegs.sect_in.feed - (SpeedRegs.spd_up_step2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step2+4);
                SpeedRegs.length = SpeedRegs.length + S_remnant_down*(S_remnant_down+1)*0.5;
            }

        }
        return SpeedRegs.length;
    }
}
// ---------------------------------------------------------------------------
// 函数功能：   定长曲线指令速度修正
// 输入参数：   段首速度int32 vs，目标速度int32 vObj，段长int32 l，
//              至最大加速度周期数int32 Nmi。
// 输出参数：   修正后的指令速度
// 创建时间：   2010
// 当前路径段终点速度预计算模块end_vel（）
// 功能描述：利用前瞻法预计算当前程序段 的终点速度
// 输入变量： 段起点速度 ，程序段 的轨线长度 ，程序段 的加速度由0 达到最大允许加速度 （或 减至0）的插补周期数 ，第 段与第 段的转接处最大允许速度 ，指令速度 ，此处
// 输出变量： 段终点速度 ， 段起点速度
// 内部变量：以下变量均在循环体中使用，即新值代替旧值。 段起点速度 ，正向速度 ，正向修正速度 ，反向修正总段数 ，反向修正速度 ，
// 调用的子函数：lim_speed（），lim_length（）
// --------------------------------------------------------------------------
int32_t Dichotomy(int32_t vs,int32_t ve, int32_t length,int32_t Nmi)
{
    int32_t Si,SUp,SDown;
    int32_t feed,feed_adj;
    feed = SpeedRegs.sect_in.feed;
    feed_adj=feed;
    //feed_adj=feed_adj*4/5;
    while(1)
    {
        feed_adj=feed_adj*4/5;
        if(feed_adj>vs && feed_adj>ve)
        {
            SUp=Lim_speed(vs,feed_adj,Nmi);
            SDown=Lim_speed(feed_adj,ve,Nmi);
            //feed_adj=feed_adj*4/5;
        }
        else
        {
            if(feed_adj<vs && feed_adj<ve)
            {
                  if(ve>vs)
                  {
                      feed_adj=ve;
                      SUp=Lim_speed(vs,feed_adj,Nmi);
                      SDown=0;
                  }
                  else
                  {
                      feed_adj=vs;
                      SUp=0;
                      SDown=Lim_speed(feed_adj,ve,Nmi);
                  }
            }
            else if(feed_adj<=ve)
            {
                  feed_adj=ve;
                  SUp=Lim_speed(vs,feed_adj,Nmi);;
                  SDown=0;
            }
            else if(feed_adj<=vs)
            {
                  feed_adj=vs;
                  SUp=0;
                  SDown=Lim_speed(feed_adj,ve,Nmi);
            }
            break;
        }
        Si = SUp + SDown;
        if(/*(labs(Si-length) < SpeedRegs.maxEr) && */( Si < length) )
        {
            break;
        }

    }
    SpeedRegs.ud_length1 = SUp;
    SpeedRegs.ud_length2 = SDown;
    return feed_adj;

}

// ---------------------------------------------------------------------------
// 函数功能：   加工段长度计算, 圆弧半径计算
// 输入参数：   当前段段矢量信息
// 输出参数：   当前段段长
// 创建时间：   2006.07.14
// 最后修改时间:2006.07.24
// 修改原因：
// 创建人  ：   付科
// 程序段长度计算模块length（）
// 功能描述：计算程序段 的轨线长度
// 输入变量： 段起点坐标 ，终点坐标 ，
// 圆弧加工类型 （0x001：直线；0x010：顺圆； 0x011：逆圆)，
// 圆弧半径 （ （劣弧）： ； （优弧）： ）
// 输出变量：程序段 的轨线长度
// 内部变量：起点与终点距离 ，圆弧轨线对应的圆心角
// 算法描述：
// （a）起点与终点距离 计算
// （b）轨线长度 计算
// 若 （直线插补类型） 则
// 若 （顺圆插补）或 （逆圆插补） 则
// ---------------------------------------------------------------------------
void speed_feed_length(struct SpeedSTRRegs *in)
{
    int32_t x0, x1, y0, y1, z0, z1, a, b ,c;
    int32_t tempi32a = 0;
    double tempfloat = 0;
    double theta = 0;
    struct INT64 temp64a;
    struct INT64 temp64b;
    struct INT64 temp64c;
    temp64a.int_h = 0;
    temp64a.int_l = 0;
    temp64b.int_h = 0;
    temp64b.int_l = 0;
    if((*in).type == 0 || (*in).type == 1)                      // 快速定位或直线插补
    {
        tempi32a = labs((*in).end.pos_x - (*in).st.pos_x);                  // X^2
        temp64a = mul64(tempi32a, tempi32a);
        tempi32a = labs((*in).end.pos_y - (*in).st.pos_y);                  // Y^2
        temp64b = mul64(tempi32a, tempi32a);
        temp64a = add64(temp64a, temp64b);              // X^2 + Y^2
        tempi32a = labs((*in).end.pos_z - (*in).st.pos_z);                  // Z^2
        temp64b = mul64(tempi32a, tempi32a);
        temp64a = add64(temp64a, temp64b);              // X^2 + Y^2 + Z^2
        tempi32a = labs((*in).end.pos_a - (*in).st.pos_a);                  // A^2
        temp64b = mul64(tempi32a, tempi32a);
        temp64a = add64(temp64a, temp64b);
        tempi32a = labs((*in).end.pos_b - (*in).st.pos_b);
        temp64b = mul64(tempi32a, tempi32a);
        temp64a = add64(temp64a, temp64b);
        tempi32a = labs((*in).end.pos_c - (*in).st.pos_c);
        temp64b = mul64(tempi32a, tempi32a);
        temp64a = add64(temp64a, temp64b);
        if(temp64a.int_h == 0)
            (*in).length = sqr32(temp64a.int_l);
        else
            (*in).length = sqr64(temp64a);
        //(*in).length = sqr64(temp64a); //shibo ceshi
    }
    else if((*in).type == 2 || (*in).type == 3)                 // 圆弧
    {
        if((*in).plane == 17)           // XY平面
        {
            x0 = (*in).st.pos_x;
            x1 = (*in).end.pos_x;
            y0 = (*in).st.pos_y;
            y1 = (*in).end.pos_y;
            z0 = (*in).st.pos_z;
            z1 = (*in).end.pos_z;
            a = (*in).i;
            b = (*in).j;
            c = (*in).k;
        }
        else if((*in).plane == 19)                                  // YZ平面
        {
            x0 = (*in).st.pos_y;
            x1 = (*in).end.pos_y;
            y0 = (*in).st.pos_z;
            y1 = (*in).end.pos_z;
            z0 = (*in).st.pos_x;
            z1 = (*in).end.pos_x;
            a = (*in).j;
            b = (*in).k;
            c = (*in).i;
        }
        else if((*in).plane == 18)                          // ZX平面
        {
            x0 = (*in).st.pos_z;
            x1 = (*in).end.pos_z;
            y0 = (*in).st.pos_x;
            y1 = (*in).end.pos_x;
            z0 = (*in).st.pos_y;
            z1 = (*in).end.pos_y;
            a = (*in).k;
            b = (*in).i;
            c = (*in).j;
        }
        temp64a = mul64((x1 - x0), (x1 - x0));
        temp64b = mul64((y1 - y0), (y1 - y0));
        temp64a = add64(temp64a, temp64b);              // X^2 + Y^2
// 弦长
        if(temp64a.int_h == 0)
            (*in).length = sqr32(temp64a.int_l);
        else
            (*in).length = sqr64(temp64a);
//(*in).length = sqr64(temp64a);
        if((*in).R == 0)                        // 半径
        {
            temp64c = add64(mul64(a, a), mul64(b, b));
            if(temp64c.int_h == 0)
                (*in).R = sqr32(temp64c.int_l);
            else
                (*in).R = sqr64(temp64c);
        }
//(*in).R = sqr64(add64(mul64(a, a), mul64(b, b)));
        tempi32a = (*in).R * 2;                         // 直径
        tempfloat = (*in).length;
        tempfloat = tempfloat / tempi32a;
        if(tempfloat>=1.0)    //防止由于计算误差引起的正弦值大于1 09.3.19LYQ
            tempfloat=1;
        theta = asin(tempfloat);
        tempfloat = tempi32a * theta;               // 2*theta*R
        (*in).length = (int32_t)tempfloat;                // 劣弧
        tempfloat = tempi32a * (pi - theta);    // (2*pi - 2*theta)*R
        if(x0!=x1 || y0!=y1 || z0!=z1 )
        {
        if(c==0)
        {
            if((*in).type == 2)                             // 顺圆
            {
                if(x0 < x1)
                {
                    if((y0 + y1) < (2 * (y0 + b)))  // 优弧
                        (*in).length = (int32_t)tempfloat;
                }
                else if(x0 > x1)
                {
                    if((y0 + y1) > (2 * (y0 + b)))  // 优弧
                        (*in).length = (int32_t)tempfloat;
                }
                else
                {
                    if(y0 > y1)
                    {
                        if(a > 0)           // 优弧
                            (*in).length = (int32_t)tempfloat;
                    }
                    else
                    {
                        if(a < 0)           // 优弧
                            (*in).length = (int32_t)tempfloat;
                    }
                }
            }
            else if((*in).type == 3)                        // 逆圆
            {
                if(x0 > x1)
                {
                    if((y0 + y1) < (2 * (y0 + b)))  // 优弧
                        (*in).length = (int32_t)tempfloat;
                }
                else if(x0 < x1)
                {
                    if((y0 + y1) > (2 * (y0 + b)))  // 优弧
                        (*in).length = (int32_t)tempfloat;
                }
                else
                {
                    if(y0 < y1)
                    {
                        if(a > 0)           // 优弧
                            (*in).length = (int32_t)tempfloat;
                    }
                    else
                    {
                        if(a < 0)           // 优弧
                            (*in).length = (int32_t)tempfloat;
                    }
                }
            }
        }
        else
        {
            if((*in).R == 0)                        // 半径
                (*in).R = sqr64(add64(mul64(a, a), mul64(b, b)));
            tempfloat = (*in).R * 2 * pi;                           // 直径
            tempfloat = tempfloat * (labs(z1-z0)) / c;
            (*in).length = (int32_t)tempfloat+1;
        }
        }
        else
        {
            (*in).length = (int32_t)tempfloat;
        }
    }
}
// ---------------------------------------------------------------------------
// 函数功能：   拐点速度计算，还可以计算出加速度至最大值所需插补周期数Nmi
// 输入参数：   当前段参数, 下一段参数
// 输出参数：   拐点速度值
// 相邻段转接处最大允许速度预计算模块turn_vel（）
// 功能描述：计算程序段 与程序段 转接点处最大允许速度
// 输入变量： 在转接点 处的单位方向矢量 ，
// 在 处的单位方向矢量为 ，
//         各轴最大进给速度 ， ，
// 输出变量：第 段与第 段的转接处最大允许速度
// 内部变量：限制速度 ，
// ---------------------------------------------------------------------------
void inflexion(void)
{
    int k;
    int32_t L1, L2;
    int32_t v1_x, v1_y, v1_z, v1_a, v1_b, v1_c;
    int32_t v2_x, v2_y, v2_z, v2_a, v2_b, v2_c;
    int32_t Fc, max_vc;//, sect_NNinLen;
    struct inter_pos e1;            // 第1段方向矢量
    struct inter_pos e2;            // 第2段方向矢量
    int32_t speed_min_x=8;
    int32_t speed_min_y=8;
    int32_t speed_min_z=8;
    int32_t speed_min_a=8;
    int32_t speed_min_b=8;
    int32_t speed_min_c=8;
    SpeedRegs.sect_Nin.length=EncodeRegs.BsReg.length;
//  sect_NNinLen=EncodeRegs.CsReg.length;
    SpeedRegs.remLen = 0;
    /*if((In_PLC2CNC_D&0x0004)==0||EncodeRegs.AsRegP!=0)  //停时
    {
        SpeedRegs.sect_in.fc=0;
        return;
    }*/
//以下计算本段及下段方向矢量
// i段方向矢量计算
    if(SpeedRegs.sect_in.type == 0 || SpeedRegs.sect_in.type == 1 || SpeedRegs.sect_in.type == 100)             // 直线插补
    {
        e1.pos_x = SpeedRegs.sect_in.end.pos_x - SpeedRegs.sect_in.st.pos_x;
        e1.pos_y = SpeedRegs.sect_in.end.pos_y - SpeedRegs.sect_in.st.pos_y;
        e1.pos_z = SpeedRegs.sect_in.end.pos_z - SpeedRegs.sect_in.st.pos_z;
        e1.pos_a = SpeedRegs.sect_in.end.pos_a - SpeedRegs.sect_in.st.pos_a;
        e1.pos_b = SpeedRegs.sect_in.end.pos_b - SpeedRegs.sect_in.st.pos_b;
        e1.pos_c = SpeedRegs.sect_in.end.pos_c - SpeedRegs.sect_in.st.pos_c;
        L1 = SpeedRegs.sect_in.length;
    }
    else
    {
        if(SpeedRegs.sect_in.type == 2)             // 顺圆插补
            k = 1;
        else if(SpeedRegs.sect_in.type == 3)
            k = -1;
        if(SpeedRegs.sect_in.R != 0)
        {
            if(SpeedRegs.sect_in.plane == 17)       // XY平面
            {
                e1.pos_x = k * (SpeedRegs.sect_in.end.pos_y - SpeedRegs.sect_in.st.pos_y - SpeedRegs.sect_in.j);
                e1.pos_y = k * (SpeedRegs.sect_in.st.pos_x + SpeedRegs.sect_in.i - SpeedRegs.sect_in.end.pos_x);
                e1.pos_z = 0;
                e1.pos_a = 0;
                e1.pos_b = 0;
                e1.pos_c = 0;
            }
            else if(SpeedRegs.sect_in.plane == 19)                      // YZ平面
            {
                e1.pos_x = 0;
                e1.pos_y = k * (SpeedRegs.sect_in.end.pos_z - SpeedRegs.sect_in.st.pos_z - SpeedRegs.sect_in.k);
                e1.pos_z = k * (SpeedRegs.sect_in.st.pos_y + SpeedRegs.sect_in.j - SpeedRegs.sect_in.end.pos_y);
                e1.pos_a = 0;
                e1.pos_b = 0;
                e1.pos_c = 0;
            }
            else if(SpeedRegs.sect_in.plane == 18)                      // ZX平面
            {
                e1.pos_x = k * (SpeedRegs.sect_in.st.pos_z + SpeedRegs.sect_in.k - SpeedRegs.sect_in.end.pos_z);
                e1.pos_y = 0;
                e1.pos_z = k * (SpeedRegs.sect_in.end.pos_x - SpeedRegs.sect_in.st.pos_x - SpeedRegs.sect_in.i);
                e1.pos_a = 0;
                e1.pos_b = 0;
                e1.pos_c = 0;
            }
        }
        else
        {
            e1.pos_x = 0;
            e1.pos_y = 0;
            e1.pos_z = 0;
            e1.pos_a = 0;
            e1.pos_b = 0;
            e1.pos_c = 0;
        }
        L1 = SpeedRegs.sect_in.R;
    }
// i+1段方向矢量计算
    if(SpeedRegs.sect_Nin.type == 0 || SpeedRegs.sect_Nin.type == 1 || SpeedRegs.sect_Nin.type == 100)              // 直线插补
    {
        e2.pos_x = SpeedRegs.sect_Nin.end.pos_x - SpeedRegs.sect_Nin.st.pos_x;
        e2.pos_y = SpeedRegs.sect_Nin.end.pos_y - SpeedRegs.sect_Nin.st.pos_y;
        e2.pos_z = SpeedRegs.sect_Nin.end.pos_z - SpeedRegs.sect_Nin.st.pos_z;
        e2.pos_a = SpeedRegs.sect_Nin.end.pos_a - SpeedRegs.sect_Nin.st.pos_a;
        e2.pos_b = SpeedRegs.sect_Nin.end.pos_b - SpeedRegs.sect_Nin.st.pos_b;
        e2.pos_c = SpeedRegs.sect_Nin.end.pos_c - SpeedRegs.sect_Nin.st.pos_c;
        L2 = SpeedRegs.sect_Nin.length;
    }
    else
    {
        if(SpeedRegs.sect_Nin.type == 2)                // 顺圆插补
            k = 1;
        else if(SpeedRegs.sect_Nin.type == 3)
            k = -1;
        if(SpeedRegs.sect_Nin.R != 0)
        {
            if(SpeedRegs.sect_Nin.plane == 17)      // XY平面
            {
                e2.pos_x = (-1) * k * SpeedRegs.sect_Nin.j;
                e2.pos_y = k * SpeedRegs.sect_Nin.i;
                e2.pos_z = 0;
                e2.pos_a = 0;
                e2.pos_b = 0;
                e2.pos_c = 0;
            }
            else if(SpeedRegs.sect_Nin.plane == 19)                     // YZ平面
            {
                e2.pos_x = 0;
                e2.pos_y = (-1) * k * SpeedRegs.sect_Nin.k;
                e2.pos_z = k * SpeedRegs.sect_Nin.j;
                e2.pos_a = 0;
                e2.pos_b = 0;
                e2.pos_c = 0;
            }
            else if(SpeedRegs.sect_Nin.plane == 18)                     // ZX平面
            {
                e2.pos_x = k * SpeedRegs.sect_Nin.k;
                e2.pos_y = 0;
                e2.pos_z = (-1) * k * SpeedRegs.sect_Nin.i;
                e2.pos_a = 0;
                e2.pos_b = 0;
                e2.pos_c = 0;
            }
        }
        else
        {
            e2.pos_x = 0;
            e2.pos_y = 0;
            e2.pos_z = 0;
            e2.pos_a = 0;
            e2.pos_b = 0;
            e2.pos_c = 0;
        }
        L2 = SpeedRegs.sect_Nin.R;
    }
// 急转判断,临界速度计算
    SpeedRegs.iNmax=NmiCal(e1,e2,L1,L2);
    /*if((In_PLC2CNC_D&0x0004)==0||EncodeRegs.AsRegP!=0)  //暂停时
    {
        SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
        return;
    }*/
    if ((SpeedRegs.sect_in.type==2) || (SpeedRegs.sect_in.type==3) || (SpeedRegs.sect_Nin.type==2) || (SpeedRegs.sect_Nin.type==3))  //存在圆弧插补
    {
        if(((e1.pos_x > 0) && (e2.pos_x > 0) || (e1.pos_x < 0) && (e2.pos_x < 0))
            && ((e1.pos_y > 0) && (e2.pos_y > 0) || (e1.pos_y < 0) && (e2.pos_y < 0))
            && ((e1.pos_z > 0) && (e2.pos_z > 0) || (e1.pos_z < 0) && (e2.pos_z < 0))
            && ((e1.pos_a > 0) && (e2.pos_a > 0) || (e1.pos_a < 0) && (e2.pos_a < 0))
            && ((e1.pos_b > 0) && (e2.pos_b > 0) || (e1.pos_b < 0) && (e2.pos_b < 0))
            && ((e1.pos_c > 0) && (e2.pos_c > 0) || (e1.pos_c < 0) && (e2.pos_c < 0)))
        {
            if(L2==0)                    //12.31 处理M代码单独的情况
            {
                SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
                return;
            }
            else
                SpeedRegs.sect_in.fc = SpeedRegs.sect_in.feed;
        }
        else
        {
            SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
            return;
        }
    }
    else//直线段反向
    {
        if(((e1.pos_x >= 0) && (e2.pos_x >= 0) || (e1.pos_x <= 0) && (e2.pos_x <= 0))
            && ((e1.pos_y >= 0) && (e2.pos_y >= 0) || (e1.pos_y <= 0) && (e2.pos_y <= 0))
            && ((e1.pos_z >= 0) && (e2.pos_z >= 0) || (e1.pos_z <= 0) && (e2.pos_z <= 0))
            && ((e1.pos_a >= 0) && (e2.pos_a >= 0) || (e1.pos_a <= 0) && (e2.pos_a <= 0))
            && ((e1.pos_b >= 0) && (e2.pos_b >= 0) || (e1.pos_b <= 0) && (e2.pos_b <= 0))
            && ((e1.pos_c >= 0) && (e2.pos_c >= 0) || (e1.pos_c <= 0) && (e2.pos_c <= 0)))
        {
            if(L2==0)
            {
                SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
                return;
            }
            else
                SpeedRegs.sect_in.fc = SpeedRegs.sect_in.feed;
// 以X轴为例：若第i段与第i+1段，其中一段输出不为0，而另一段输出为0，则SpeedRegs.sect_in.fc=0
            if (((e1.pos_x*e2.pos_x==0)&&(e1.pos_x+e2.pos_x!=0)) || ((e1.pos_y*e2.pos_y==0)&&(e1.pos_y+e2.pos_y!=0)) || ((e1.pos_z*e2.pos_z==0)&&(e1.pos_z+e2.pos_z!=0)) || ((e1.pos_a*e2.pos_a==0)&&(e1.pos_a+e2.pos_a!=0)) || ((e1.pos_b*e2.pos_b==0)&&(e1.pos_b+e2.pos_b!=0)) || ((e1.pos_c*e2.pos_c==0)&&(e1.pos_c+e2.pos_c!=0)))
            {
                SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
                return;
            }
        }
        else
        {
            SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
            return;
        }
    }

    Fc = SpeedRegs.sect_in.feed;
    if (e1.pos_x==0)    // x轴
        v1_x=0;
    else
        v1_x=e1.pos_x*Fc/L1;
    if (e2.pos_x==0)
        v2_x=0;
    else
        v2_x=e2.pos_x*Fc/L2;
    v1_x = labs(v1_x-v2_x);
    v1_x = v1_x/speed_min_x;

    if (e1.pos_y==0)    // y轴
        v1_y=0;
    else
        v1_y=e1.pos_y*Fc/L1;
    if (e2.pos_y==0)
        v2_y=0;
    else
        v2_y=e2.pos_y*Fc/L2;
    v1_y = labs(v1_y-v2_y);
    v1_y = v1_y/speed_min_y;

    if (e1.pos_z==0)    // z轴
        v1_z=0;
    else
        v1_z=e1.pos_z*Fc/L1;
    if (e2.pos_z==0)
        v2_z=0;
    else
        v2_z=e2.pos_z*Fc/L2;
    v1_z = labs(v1_z-v2_z);
    v1_z = v1_z/speed_min_z;

    if (e1.pos_a==0)    // a轴
        v1_a=0;
    else
        v1_a=e1.pos_a*Fc/L1;
    if (e2.pos_a==0)
        v2_a=0;
    else
        v2_a=e2.pos_a*Fc/L2;
    v1_a = labs(v1_a-v2_a);
    v1_a = v1_a/speed_min_a;

    if (e1.pos_b==0)    // b
        v1_b=0;
    else
        v1_b=e1.pos_b*Fc/L1;
    if (e2.pos_b==0)
        v2_b=0;
    else
        v2_b=e2.pos_b*Fc/L2;
    v1_b = labs(v1_b-v2_b);
    v1_b = v1_b/speed_min_b;

    if (e1.pos_c==0)    // c轴
        v1_c=0;
    else
        v1_c=e1.pos_c*Fc/L1;
    if (e2.pos_c==0)
        v2_c=0;
    else
        v2_c=e2.pos_c*Fc/L2;
    v1_c = labs(v1_c-v2_c);
    v1_c = v1_c/speed_min_c;

    if((v1_x>0) || (v1_y>0) || (v1_z>0)||(v1_a>0) || (v1_b>0) || (v1_c>0))
    {
       // max_vc = v1_x;//cl
        max_vc = 999999;//cl2013.11.5 去掉了速度保护
        if(v1_y>max_vc)
            max_vc = v1_y;
        if(v1_z>max_vc)
            max_vc = v1_z;
        if(v1_a>max_vc)
            max_vc = v1_a;
        if(v1_b>max_vc)
            max_vc = v1_b;
        if(v1_c>max_vc)
            max_vc = v1_c;
        Fc = Fc/(max_vc+1);
    }
    SpeedRegs.sect_in.fc = Fc;
    if(SpeedRegs.sect_Nin.length!=0)
    {
        while((Lim_speed(SpeedRegs.sect_in.fc,SpeedRegs.speed_min,SpeedRegs.iNmax)) > SpeedRegs.sect_Nin.length)
        {
            SpeedRegs.sect_in.fc = (SpeedRegs.sect_in.fc*4/5);
            if(SpeedRegs.sect_in.fc<SpeedRegs.speed_min)
                SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
        }
    }
    if(SpeedRegs.sect_in.fc<SpeedRegs.speed_min)
        SpeedRegs.sect_in.fc = SpeedRegs.speed_min;
}
// ---------------------------------------------------------------------------
// 函数功能：  当前渭蛹跛偾砺鄢ざ燃扑�
// 输入参数：   当前段参数, 上一段拐点速度
// 输出参数：   当前段理论加速段长及砺奂跛俣纬�
// 加减速理论长度计算模块lim_length（）
// 功能描述：根据加（减）速过程的起点速度和终点速度，加速度由0 达到最大值 （或 减至0）的插补周期数 ，计算达到终点速度所需的最小长度
// 输入变量：加（减）速起点速度 ，终点速度 ， ， ，
// 输出变量：加（减）速过程的理论长度
// 内部变量： ， ， ， ， ， ，

// ---------------------------------------------------------------------------
void speed_ud_length(int32_t CompVart_Flag)
{
    // 段首加减速处理length1
    if(SpeedRegs.sect_Lin.fc >= (SpeedRegs.sect_in.feed - 2*SpeedRegs.Epslion) && SpeedRegs.sect_Lin.fc <= (SpeedRegs.sect_in.feed + 2*SpeedRegs.Epslion))          // 上段段尾末速度 = 该段目标速度,段首不处理
    {
        SpeedRegs.ud_length1 = 0;
        SpeedRegs.ud_stepNum1 = 0;
        SpeedRegs.sect_in.flag = 0;
    }
    else
    {
        // 计算加速区间长度
        if(SpeedRegs.sect_in.type == 0)                 // 快速定位,段首加速
        {
            SpeedRegs.ud_stepNum1 = labs(SpeedRegs.sect_Lin.fc - SpeedRegs.sect_in.feed) / SpeedRegs.KL;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        SpeedRegs.ud_length1 = SpeedRegs.sect_Lin.fc*SpeedRegs.sect_in.feed/SpeedRegs.KL+SpeedRegs.sect_in.feed / 2 + SpeedRegs.sect_in.feed * SpeedRegs.sect_in.feed / SpeedRegs.KL / 2;
            SpeedRegs.ud_length1 = SpeedRegs.ud_stepNum1*SpeedRegs.sect_Lin.fc+ SpeedRegs.ud_stepNum1*(SpeedRegs.ud_stepNum1+1)*SpeedRegs.KL/2;
        }
        else
        {
            if(CompVart_Flag==1)
                SpeedRegs.ud_length1 = Lim_speed(SpeedRegs.sect_Lin.fc, SpeedRegs.sect_in.feed,SpeedRegs.iNmax);// 段首加速长度
        }
        if (SpeedRegs.ud_length1!=0)
            SpeedRegs.sect_in.flag = 1;                                 // 段首加速标志
    }
    // 段尾加减速处理length2
    if(SpeedRegs.sect_in.fc >= (SpeedRegs.sect_in.feed - 2*SpeedRegs.Epslion) && SpeedRegs.sect_in.fc <= (SpeedRegs.sect_in.feed + 2*SpeedRegs.Epslion))   // 段尾不处理
    {
        SpeedRegs.ud_length2 = 0;
        SpeedRegs.ud_stepNum2 = 0;
        if(SpeedRegs.sect_in.flag == 1)     // 段首加减速,段尾无加减速
            SpeedRegs.sect_in.flag = 10;
        else
            SpeedRegs.sect_in.flag = 0;     // 首尾都无加减速
    }
    else
    {
        if(SpeedRegs.sect_in.type == 0)         // 快速定位,段尾减速
        {
            SpeedRegs.ud_stepNum2 = labs(SpeedRegs.sect_in.fc - SpeedRegs.sect_in.feed) / SpeedRegs.KL;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     SpeedRegs.ud_length1 = SpeedRegs.sect_Lin.fc*SpeedRegs.sect_in.feed/SpeedRegs.KL+SpeedRegs.sect_in.feed / 2 + SpeedRegs.sect_in.feed * SpeedRegs.sect_in.feed / SpeedRegs.KL / 2;
            SpeedRegs.ud_length2 = SpeedRegs.ud_stepNum1*SpeedRegs.sect_Lin.fc- SpeedRegs.ud_stepNum1*(SpeedRegs.ud_stepNum1+1)*SpeedRegs.KL/2;
        }
        else
        {
            if(CompVart_Flag==1)
            {
                if(SpeedRegs.sect_in.fc!=0)
                    SpeedRegs.ud_length2 = Lim_speed(SpeedRegs.sect_in.feed, SpeedRegs.sect_in.fc,SpeedRegs.iNmax);// 段尾加速长度
                else
                    SpeedRegs.ud_length2 = Lim_speed(SpeedRegs.sect_in.feed, SpeedRegs.speed_min,SpeedRegs.iNmax);// 段尾加速长度Wuu
            }
        }
        if (SpeedRegs.ud_length2!=0)

        {
            if(SpeedRegs.sect_in.flag == 1)
                SpeedRegs.sect_in.flag = 11;                                // 段首加减速,段尾加速速
            else
                SpeedRegs.sect_in.flag = 01;
        }                                   // 段首无加减速,段尾加速速
    }
}

// 加减速初始化
// 计算段长、段末速度、加速段长度、减速段长度、
void Speed_Control0(void)   // 加减速初始化
{
    int32_t CircleFeed;
    int32_t L2,delta_feed,step_feed,adjust_feed1,adjust_feed2,Len_Step3_length;
    CircleFeed = 130;
    SpeedRegs.sect_in_fc=0;
    SpeedRegs.count_w1=0;
    SpeedRegs.ud_length1=0;
    SpeedRegs.ud_length2=0;
    if(SpeedRegs.speedini_count==0)      // first
    {
        SpeedRegs.Tm=40;//40;//40; max 96                                 // 插补周期
        SpeedRegs.ax=SpeedRegs.ay=SpeedRegs.az=SpeedRegs.aa=SpeedRegs.ab=SpeedRegs.ac=20;//20;   // 加速度
        Speed_init(SpeedRegs.Tm,SpeedRegs.ax,SpeedRegs.ay,SpeedRegs.az,SpeedRegs.aa,SpeedRegs.ab,SpeedRegs.ac);
        SpeedRegs.speedini_count=1;
    }
    if(EncodeRegs.AsReg.DaoFlag==1)          //刀补或者固定循环时计算段长 01.07
        speed_feed_length(&SpeedRegs.sect_in);  // 当前段段长预计算
    else
        SpeedRegs.sect_in.length=EncodeRegs.AsReg.length;
    if(SpeedRegs.sect_in.length<=0)
    {
        SpeedRegs.updown_stats=6;
        SpeedRegs.rem_length = 0;
        return;
    }
    SpeedRegs.rem_length = SpeedRegs.sect_in.length;           // 剩余段长
    if(SpeedRegs.sect_Nin.type == -2 || SpeedRegs.sect_in.type == 0 || SpeedRegs.sect_in.S != SpeedRegs.sect_Nin.S)
    {
        SpeedRegs.sect_in.fc = SpeedRegs.speed_min;
        SpeedRegs.iNmax=SpeedRegs.Nmax;
    }
    else      // 下段
    {
        if(EncodeRegs.AsReg.DaoFlag==1)
        {
            SpeedRegs.sect_in.fc = SpeedRegs.speed_min;
            SpeedRegs.iNmax=SpeedRegs.Nmax;
        }
        else
        {
            if(SpeedRegs.sect_in.feed>CircleFeed && (SpeedRegs.sect_in.type==2||SpeedRegs.sect_in.type==3))
                SpeedRegs.sect_in.feed = CircleFeed;
            inflexion();   // 计算程序段 与程序段 转接点处最大允许速度(SpeedRegs.sect_in.fc)和加速度至最大值所需插补周期数Nmi(SpeedRegs.iNmax)
        }
    }
    if (SpeedRegs.sect_in.feed<SpeedRegs.sect_Lin.fc)
        SpeedRegs.sect_in.feed=SpeedRegs.sect_Lin.fc;
    else if (SpeedRegs.sect_in.feed-SpeedRegs.sect_Lin.fc<SpeedRegs.speed_min)
        SpeedRegs.sect_in.feed=SpeedRegs.sect_Lin.fc;
    if (SpeedRegs.sect_in.feed<SpeedRegs.speed_min && SpeedRegs.sect_Lin.fc<SpeedRegs.speed_min)
        SpeedRegs.sect_in.feed=SpeedRegs.sect_Lin.fc=SpeedRegs.speed_min;
    if(SpeedRegs.sect_in.feed<SpeedRegs.speed_min)
        SpeedRegs.sect_in.feed=SpeedRegs.speed_min;
    if(SpeedRegs.sect_in.feed<SpeedRegs.sect_in.fc)
        SpeedRegs.sect_in.fc=SpeedRegs.sect_in.feed;
    if(SpeedRegs.sect_Lin.fc<SpeedRegs.speed_min)//chenlin2014.5.23
        SpeedRegs.sect_Lin.fc=SpeedRegs.speed_min;
    speed_ud_length(1);// 当前段加减速区间长度预扑?  计算加（减）速过程的理论长度ud_length1、ud_length2
// 加减速区间重新计算，段长不走加减速段//初速度为v0，指令速度为F，末速度为ve，段长为L
    if ( (SpeedRegs.sect_in.length < (SpeedRegs.ud_length1 + SpeedRegs.ud_length2)) && (SpeedRegs.sect_in.length!=0))
    {
        if(SpeedRegs.sect_in.fc!=0)   //末速度不等于0
        {
            if (SpeedRegs.ud_length1==0)                         //无加速段；F->ve（此时v0=F)
            {
                if(SpeedRegs.ud_length2>SpeedRegs.sect_in.length)//  // 减速段>段长度   无加速段只有减速段
                {
                    SpeedRegs.sect_in.fc = (SpeedRegs.sect_in.fc*5/4);  // 速度降为
                    while( (SpeedRegs.ud_length2=(Lim_speed(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.fc,SpeedRegs.iNmax))) > SpeedRegs.sect_in.length)
                    {
                        SpeedRegs.sect_in.fc = (SpeedRegs.sect_in.fc*5/4);
                        if(SpeedRegs.sect_in.fc>SpeedRegs.sect_Lin.fc)
                        {
                            SpeedRegs.sect_in.fc=SpeedRegs.sect_Lin.fc;
                            SpeedRegs.ud_length2=0;
                            break;
                        }
                    }
                }
                speed_ud_length(0);             //  计算加（减）速过程的理论长度ud_length1、ud_length2
            }
            else if(SpeedRegs.ud_length2==0)  // 只有加速段，无减速段：v0->F（此时ve=F)
            {
                if(SpeedRegs.ud_length1>SpeedRegs.sect_in.length) // 加速段>段长度
                {
                    SpeedRegs.sect_in.feed = (SpeedRegs.sect_in.feed*4/5);
                    while((SpeedRegs.ud_length1=(Lim_speed(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.feed,SpeedRegs.iNmax))) > SpeedRegs.sect_in.length)
                    {
                        SpeedRegs.sect_in.feed = (SpeedRegs.sect_in.feed*4/5);
                        if(SpeedRegs.sect_in.feed<SpeedRegs.sect_Lin.fc)
                        {
                            SpeedRegs.sect_in.feed=SpeedRegs.sect_Lin.fc;
                            SpeedRegs.ud_length1=0;
                            break;
                        }
                    }
                }
                SpeedRegs.sect_in.fc=SpeedRegs.sect_in.feed;
                speed_ud_length(0);                 // 计算加（减）速过程的理论长度ud_length1、ud_length2
            }
            else if(SpeedRegs.ud_length2!=0 && SpeedRegs.ud_length1!=0 )  // 有加速段和减速段
            {
                Len_Step3_length=Lim_speed(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.fc,SpeedRegs.iNmax);
                if(SpeedRegs.sect_in.length==Len_Step3_length)
                {
                    SpeedRegs.sect_in.feed=SpeedRegs.sect_in.fc;
                    speed_ud_length(0);          //  计算加（减）速过程的理论长度ud_length1、ud_length2
                }
                else if (SpeedRegs.sect_in.length<Len_Step3_length)  //只有减速：v0->ve
                {
                    if(SpeedRegs.sect_in.fc<SpeedRegs.sect_Lin.fc)
                    {
                        SpeedRegs.sect_in.fc = (SpeedRegs.sect_in.fc*5/4);//有问题
                        while((SpeedRegs.ud_length2=(Lim_speed(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.fc,SpeedRegs.iNmax))) > SpeedRegs.sect_in.length)
                        {
                            SpeedRegs.sect_in.fc = (SpeedRegs.sect_in.fc*5/4);
                            if(SpeedRegs.sect_in.fc>SpeedRegs.sect_Lin.fc)
                            {
                                SpeedRegs.sect_in.fc=SpeedRegs.sect_Lin.fc;
                                break;
                            }
                        }
                        SpeedRegs.ud_length1=0;
                        SpeedRegs.sect_in.feed=SpeedRegs.sect_Lin.fc;
                    }
                    else if(SpeedRegs.sect_Lin.fc<SpeedRegs.sect_in.fc)
                    {
                        SpeedRegs.sect_in.fc = (SpeedRegs.sect_in.fc*4/5);
                        while((SpeedRegs.ud_length1=(Lim_speed(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.fc,SpeedRegs.iNmax))) > SpeedRegs.sect_in.length)
                        {
                            SpeedRegs.sect_in.fc = (SpeedRegs.sect_in.fc*4/5);
                            if(SpeedRegs.sect_in.fc<SpeedRegs.sect_Lin.fc)
                            {
                                SpeedRegs.sect_in.fc=SpeedRegs.sect_Lin.fc;
                                break;
                            }
                        }
                        SpeedRegs.ud_length2=0;
                        SpeedRegs.sect_in.feed=SpeedRegs.sect_in.fc;
                    }
                    speed_ud_length(0);          // 计算加（减）速过程的理论长度ud_length1、ud_length2
                }
                else if(SpeedRegs.sect_in.length>Len_Step3_length)//重新计算新的噶速度：F_New
                {
                    SpeedRegs.sect_in.feed=Dichotomy(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.fc,SpeedRegs.sect_in.length,SpeedRegs.iNmax);
                    speed_ud_length(0);         // 计算加（减）速过程的理论长度ud_length1、ud_length2
                }
            }
        }
        else  //末速度等于0
        {
            SpeedRegs.sect_in.fc=SpeedRegs.speed_min;//Wuu
            if (SpeedRegs.ud_length1==0)   //只有减速段；F->ve（此时v0=F)
            {
                //speed_ud_length(1);// 当前段加减速区间长度预扑?
            }
            else
            {
                Len_Step3_length=Lim_speed(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.fc,SpeedRegs.iNmax);
                if (SpeedRegs.sect_in.length<=Len_Step3_length)  //只有减速：v0->ve
                {
                    SpeedRegs.sect_in.feed=SpeedRegs.sect_Lin.fc;
                    speed_ud_length(1);                       // 计算加（减）速过程的理论长度ud_length1、ud_length2
                }
                else //重新计算新的指令速度：F_New
                {
                    if(SpeedRegs.sect_in.feed<SpeedRegs.sect_Lin.fc)
                    {
                        SpeedRegs.sect_in.feed=SpeedRegs.sect_Lin.fc;
                        speed_ud_length(1);                  // 计算加（减）速过程的理论长度ud_length1、ud_length2
                    }
                    else
                    {
                        SpeedRegs.sect_in.feed=Dichotomy(SpeedRegs.sect_Lin.fc,SpeedRegs.sect_in.fc,SpeedRegs.sect_in.length,SpeedRegs.iNmax);
                        speed_ud_length(0);                  // 计算加（减）速过程的理论长度ud_length1、ud_length2
                    }
                }
            }
        }
    }
    else
    {
        if(SpeedRegs.sect_in.fc<SpeedRegs.speed_min)
           SpeedRegs.sect_in.fc=SpeedRegs.speed_min;
    }
    // 速度规划
    L2=SpeedRegs.sect_in.length-SpeedRegs.ud_length1-SpeedRegs.ud_length2;//L2为匀速段长度
    SpeedRegs.adjust_feed_flag=0;   // 匀速段速度调整标志，=1时需调整，为全局变量
    SpeedRegs.delta_feed_cnt=0;     //余数次数记数器
    if (L2>0)                       // 有匀速段
    {
        delta_feed=L2%SpeedRegs.sect_in.feed;//求出匀速段余量，delta_feed为匀速段余量
        step_feed=(L2-delta_feed)/SpeedRegs.sect_in.feed;//除去余量，剩下的需要走的步数，step_feed为除去余量后需要走的步数
        if (SpeedRegs.ud_length1>0)        // 有匀速段有加速段
        {
            int32_t adjust_feed_temp,adjust_feed_temp1;
            SpeedRegs.adjust_feed=delta_feed/SpeedRegs.spd_up_step;//余量放到加速段后，加速段每步需要加的余量
            if(SpeedRegs.ud_length2>0)       // 有匀速段,有加速段,有减速段
            {
                adjust_feed_temp1=delta_feed/SpeedRegs.spd_down_step; //余量放到减速段后，减速段每步需要加的余量
                adjust_feed1=L2/(step_feed+1);  //step_feed+1步完成匀速段//求出step_feed+1步走完匀速段每步的大小
                if(step_feed==0)
                    adjust_feed2=adjust_feed1;
                else
                    adjust_feed2=L2/step_feed;  //step_feed步完成匀速段//求出step_feed步走完匀速段每步的大小
                if ((SpeedRegs.sect_in.feed-adjust_feed1)<(adjust_feed2-SpeedRegs.sect_in.feed))//2009.03.05LYQ
                    adjust_feed_temp=adjust_feed1;
                else
                    adjust_feed_temp=adjust_feed2;   //adjust_feed_temp为调整后的匀速段速度
                if((labs(adjust_feed_temp-SpeedRegs.sect_in.feed))>SpeedRegs.adjust_feed)
                {
                    if(SpeedRegs.adjust_feed<adjust_feed_temp1)
                    {
                        SpeedRegs.adjust_feed_flag=1;  //加速段进行
                        SpeedRegs.delta_feed_cnt=delta_feed%SpeedRegs.spd_up_step;
                        SpeedRegs.ud_length1=SpeedRegs.ud_length1+delta_feed-2;
                    }
                    else
                    {
                        SpeedRegs.adjust_feed_flag=3;  //减速段进行
                        SpeedRegs.adjust_feed=adjust_feed_temp1;
                        SpeedRegs.delta_feed_cnt=delta_feed%SpeedRegs.spd_down_step;
                        SpeedRegs.ud_length2=SpeedRegs.ud_length2+delta_feed+2;
                    }
                }
                else//放到匀速段进行补偿
                {
                    if((labs(adjust_feed_temp-SpeedRegs.sect_in.feed))<=adjust_feed_temp1)//chenlin2014.6.21
                    {
                        SpeedRegs.adjust_feed_flag=2;  //匀速段进行
                        SpeedRegs.Sect_in_feed=adjust_feed_temp;
                        SpeedRegs.delta_feed_cnt=L2%SpeedRegs.Sect_in_feed;  //余数次数记数器（全局变量）
                        SpeedRegs.adjust_feed=0;
                    }
                    else
                    {
                        SpeedRegs.adjust_feed_flag=3;  //减速段进行
                        SpeedRegs.adjust_feed=adjust_feed_temp1;
                        SpeedRegs.delta_feed_cnt=delta_feed%SpeedRegs.spd_down_step;
                        SpeedRegs.ud_length2=SpeedRegs.ud_length2+delta_feed+2;
                    }
                }
            }
            else    // 有匀速段,有加速段,无减速段
            {
                adjust_feed1=L2/(step_feed+1);  //step_feed+1步完成匀速段
                if(step_feed==0)
                    adjust_feed2=adjust_feed1;
                else
                    adjust_feed2=L2/step_feed;  //step_feed步完成匀速段
                if ((SpeedRegs.sect_in.feed-adjust_feed1)<(adjust_feed2-SpeedRegs.sect_in.feed))//2009.03.05LYQ
                    adjust_feed_temp=adjust_feed1;
                else
                    adjust_feed_temp=adjust_feed2;   //adjust_feed_temp为髡竽指令速度，为全局变量
                if((labs(adjust_feed_temp-SpeedRegs.sect_in.feed))>SpeedRegs.adjust_feed)
                {
                    SpeedRegs.adjust_feed_flag=1;  //加速段进行
                    SpeedRegs.delta_feed_cnt=delta_feed%SpeedRegs.spd_up_step;
                    SpeedRegs.ud_length1=SpeedRegs.ud_length1+delta_feed-2;
                }
                else
                {
                    SpeedRegs.adjust_feed_flag=2;  //匀速段进行
                    SpeedRegs.Sect_in_feed=adjust_feed_temp;
                    SpeedRegs.delta_feed_cnt=L2%SpeedRegs.Sect_in_feed;  //余数次数记数器（全局变量）
                    SpeedRegs.adjust_feed=0;
                }
            }
        }
        else if (SpeedRegs.ud_length2>0)          // 有匀速段,无加速段，有减速段
        {
            int32_t adjust_feed_temp;
            SpeedRegs.adjust_feed=delta_feed/SpeedRegs.spd_down_step;
            adjust_feed1=L2/(step_feed+1);  //step_feed+1步完成匀速段
            if(step_feed==0)
                adjust_feed2=adjust_feed1;
            else
                adjust_feed2=L2/step_feed;  //step_feed步完成匀速段
            if ((SpeedRegs.sect_in.feed-adjust_feed1)<(adjust_feed2-SpeedRegs.sect_in.feed))//2009.03.05LYQ
                adjust_feed_temp=adjust_feed1;
            else
                adjust_feed_temp=adjust_feed2;   //adjust_feed_temp为调整后的指令速度，为全局变量
            if((labs(adjust_feed_temp-SpeedRegs.sect_in.feed))>SpeedRegs.adjust_feed)
            {
                SpeedRegs.adjust_feed_flag=3;  //减速段进行
                SpeedRegs.delta_feed_cnt=delta_feed%SpeedRegs.spd_down_step;
                SpeedRegs.ud_length2=SpeedRegs.ud_length2+delta_feed+2;
            }
            else
            {
                SpeedRegs.adjust_feed_flag=2;  //匀速段进行
                SpeedRegs.Sect_in_feed=adjust_feed_temp;
                SpeedRegs.delta_feed_cnt=L2%SpeedRegs.Sect_in_feed;  //余数次数记数器（全局变量）
                SpeedRegs.adjust_feed=0;
            }
        }
        else         // 有匀速段,无加速段，无减速段
        {
            SpeedRegs.adjust_feed_flag=2;  //匀速段进行
            adjust_feed1=L2/(step_feed+1);  //step_feed+1步完成匀速段
            if(step_feed==0)
                adjust_feed2=adjust_feed1;
            else
                adjust_feed2=L2/step_feed;  //step_feed步完成匀速段
            if ((SpeedRegs.sect_in.feed-adjust_feed1)<(adjust_feed2-SpeedRegs.sect_in.feed))//2009.03.05LYQ
                SpeedRegs.adjust_feed=adjust_feed1;
            else
                SpeedRegs.adjust_feed=adjust_feed2;   //adjust_feed为调整后的指令速度，为全局变量
            SpeedRegs.Sect_in_feed=SpeedRegs.adjust_feed;
            SpeedRegs.delta_feed_cnt=L2%SpeedRegs.Sect_in_feed;  //余数次数记数器（全局变量）
            SpeedRegs.adjust_feed=0;
        }
    }
    else    // 无匀速段
    {
        if (SpeedRegs.ud_length1>0)    // 无匀速段，有加速段
        {
            SpeedRegs.adjust_feed_flag=1;  //加速段进行速度规划
            SpeedRegs.adjust_feed=delta_feed/SpeedRegs.spd_up_step;
            SpeedRegs.delta_feed_cnt=delta_feed%SpeedRegs.spd_up_step;
            SpeedRegs.ud_length1=SpeedRegs.ud_length1+delta_feed-2;
        }
        else if (SpeedRegs.ud_length2>0)    // 无匀速段，无加速段，有减速段
        {
            SpeedRegs.adjust_feed_flag=3;  //减速段进行
            SpeedRegs.adjust_feed=delta_feed/SpeedRegs.spd_down_step;
            SpeedRegs.delta_feed_cnt=delta_feed%SpeedRegs.spd_down_step;
            SpeedRegs.ud_length2=SpeedRegs.ud_length2+delta_feed+2;
        }
    }
    SpeedRegs.O_feed1 = SpeedRegs.sect_in.feed;
    if(SpeedRegs.sect_in.flag == 10)                // 首有,尾无
    {
        SpeedRegs.sect_in.vi = SpeedRegs.sect_Lin.fc;
        SpeedRegs.O_feed2 = SpeedRegs.sect_in.feed;
        SpeedRegs.updown_stats = 1;
    }
    else if(SpeedRegs.sect_in.flag == 01)       // 首无,尾有
    {
        SpeedRegs.sect_in.vi = SpeedRegs.sect_in.feed;
        SpeedRegs.O_feed2 = SpeedRegs.sect_in.fc;
        SpeedRegs.updown_stats = 3;
    }
    else if(SpeedRegs.sect_in.flag == 11)       // 首尾都有
    {
        SpeedRegs.sect_in.vi = SpeedRegs.sect_Lin.fc;
        SpeedRegs.O_feed2 = SpeedRegs.sect_in.fc;
        SpeedRegs.updown_stats = 1;
    }
    else                                        // 首尾都无
    {
        SpeedRegs.sect_in.vi = SpeedRegs.sect_in.feed;
        SpeedRegs.O_feed2 = SpeedRegs.sect_in.feed;
        SpeedRegs.updown_stats = 3;
    }
}
void Speed_Control1(void)   // 段首加减速(加速段)
{
    SpeedRegs.count_w1++;
    if((SpeedRegs.sect_in.length - SpeedRegs.rem_length) >= SpeedRegs.ud_length1)   // 段首加速已完成  //chenlin2014.5.29beifen
    {
        SpeedRegs.updown_stats = 3;
    }
    else
    {
        if(SpeedRegs.sect_in.type == 0)         // 快速定位,段首线性加速
        {
            SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.KL;
            if(SpeedRegs.sect_in.vi >= SpeedRegs.O_feed1)   // 限制 SpeedRegs.sect_in.vi 最大值
                SpeedRegs.sect_in.vi = SpeedRegs.O_feed1;
        }
        else
        {
            if(SpeedRegs.sect_Lin.fc == SpeedRegs.speed_min || SpeedRegs.sect_in.feed == SpeedRegs.speed_min)//chenlin2014.7.1
            {
                if( SpeedRegs.sect_in.feed == SpeedRegs.spd_up_step2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step2+4 )
                {
                    if(SpeedRegs.count_w1==1)//chenlin2014.5.26beifen
                        SpeedRegs.sect_in.vi = SpeedRegs.speed_min;
                    if( SpeedRegs.count_w1>1  && SpeedRegs.count_w1<(SpeedRegs.spd_up_step2+2))  //按指数规律计算速度//chenlin2014.5.26beifen
                        SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (SpeedRegs.count_w1-2)*SpeedRegs.JJSpeed;
                    if((SpeedRegs.spd_up_step3!=0) && ((SpeedRegs.spd_up_step2+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+2)))
                        SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + ((SpeedRegs.spd_up_step2)-1)*SpeedRegs.JJSpeed;
                    if(((SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+2)))
                        SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1-SpeedRegs.count_w1)*SpeedRegs.JJSpeed;
                }
                else if( SpeedRegs.sect_in.feed > SpeedRegs.spd_up_step2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step2+4 )//chenlin2014.5.29
                {
                    if(SpeedRegs.count_w1==1)
                    {
                        SpeedRegs.sect_in.vi = SpeedRegs.speed_min;
                        SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + 1 ;
                        S_remnant_up = S_remnant_up - 1;
                    }
                    if( SpeedRegs.count_w1>1  && SpeedRegs.count_w1<(SpeedRegs.spd_up_step2+2) && S_remnant_up >0 )  //按指数规律计算速度
                        {
                            SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (SpeedRegs.count_w1-2)*SpeedRegs.JJSpeed;
                            SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + 1 ;
                            S_remnant_up = S_remnant_up - 1;
                        }
                    else if( SpeedRegs.count_w1>1  && SpeedRegs.count_w1<(SpeedRegs.spd_up_step2+2) )
                            SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (SpeedRegs.count_w1-2)*SpeedRegs.JJSpeed;

                    if(((SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+2)) && S_remnant_up >0 )
                        {   SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1-SpeedRegs.count_w1)*SpeedRegs.JJSpeed;
                            SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi +1;
                            S_remnant_up = S_remnant_up - 1;
                        }
                    else if( ((SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+2)) )
                            SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1-SpeedRegs.count_w1)*SpeedRegs.JJSpeed;

                }
            }
            else //chenlin2014.7.1
            {
                if( SpeedRegs.count_w1>1  && SpeedRegs.count_w1<(SpeedRegs.spd_up_step2+2))  //按指数规律计算速度//chenlin2014.5.26beifen
                    SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (SpeedRegs.count_w1-2)*SpeedRegs.JJSpeed;
                if((SpeedRegs.spd_up_step3!=0) && ((SpeedRegs.spd_up_step2+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+2)))
                    SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + ((SpeedRegs.spd_up_step2)-1)*SpeedRegs.JJSpeed;
                if(((SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+2)))
                    SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_up + (2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step3+1-SpeedRegs.count_w1)*SpeedRegs.JJSpeed;

            }
        }
        if(SpeedRegs.adjust_feed_flag==1)     // 加速段调整剩余脉冲
        {
            if(SpeedRegs.delta_feed_cnt>=1)
            {
                SpeedRegs.adjust_feed_BK=SpeedRegs.sect_in.vi + SpeedRegs.adjust_feed+1;
                SpeedRegs.delta_feed_cnt--;
            }
            else if(SpeedRegs.delta_feed_cnt<0)
            {
                SpeedRegs.sect_in_feed_BK=SpeedRegs.sect_in.vi + SpeedRegs.adjust_feed-1;
                SpeedRegs.delta_feed_cnt++;
            }
            else
            {
                SpeedRegs.sect_in_feed_BK = SpeedRegs.sect_in.vi + SpeedRegs.adjust_feed;
            }
        }
        else
            SpeedRegs.sect_in_feed_BK = SpeedRegs.sect_in.vi;
        ud_length1_true = ud_length1_true + SpeedRegs.sect_in.vi;//chenlin2014.5.29测试
    }
}
void Speed_Control3(void)   // 段中加减速（匀速段）
{
    if(ud_length1_true != 0)
    {
        ud_length1_true = 0;    //chenlin2014.5.28
    }
    if(SpeedRegs.rem_length <= SpeedRegs.ud_length2 + 0.3 * SpeedRegs.sect_in_feed_BK ) //chenlin2014.5.27
    {
        SpeedRegs.updown_stats = 5;
        SpeedRegs.count_w1=0;
        if(SpeedRegs.sect_in.vi!=SpeedRegs.sect_in.feed)
            SpeedRegs.sect_in.vi= SpeedRegs.sect_in.feed;
    }
    else
    {
        if (SpeedRegs.adjust_feed_flag==2)   // 匀速段进行剩余脉冲
        {
            if (SpeedRegs.delta_feed_cnt>=1)
            {
                SpeedRegs.sect_in_feed_BK = SpeedRegs.Sect_in_feed+1;
                SpeedRegs.delta_feed_cnt--;
            }
            else if(SpeedRegs.delta_feed_cnt<0)
            {
                SpeedRegs.sect_in_feed_BK=SpeedRegs.Sect_in_feed-1;
                SpeedRegs.delta_feed_cnt++;
            }
            else
                SpeedRegs.sect_in_feed_BK = SpeedRegs.Sect_in_feed/*SpeedRegs.Sect_in_feed*/;  //对匀速段速度做调整
        }
        else
        {
            SpeedRegs.sect_in.vi = SpeedRegs.O_feed1;
            SpeedRegs.sect_in_feed_BK=SpeedRegs.sect_in.vi;
        }
    }
}
void Speed_Control5(void)    // 段尾加减速(减速段)
{
    SpeedRegs.count_w1++;
    if((EncodeRegs.AsReg.Type == 2 || EncodeRegs.AsReg.Type ==3) && RunstatusRegs.circle_MDIstop_flag == 1)
        RunstatusRegs.circle_MDIstop_flag = 2;
    if(SpeedRegs.sect_in.type == 0)       // G00
        SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi - SpeedRegs.KL;
    else
    {
        if( SpeedRegs.sect_in.feed == SpeedRegs.spd_up_step2*SpeedRegs.spd_up_step2+SpeedRegs.spd_up_step2+4 )
        {
            if(SpeedRegs.count_w1>1 && SpeedRegs.count_w1<(SpeedRegs.spd_down_step2+2))  //按指数规律计算速度
                SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (SpeedRegs.count_w1-2)*SpeedRegs.JJSpeed;
            if((SpeedRegs.spd_down_step3!=0) && ((SpeedRegs.spd_down_step2+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+2)))
                SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (SpeedRegs.spd_down_step2-1)*SpeedRegs.JJSpeed;
            if(((SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+2)))
                SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+1-SpeedRegs.count_w1)*SpeedRegs.JJSpeed;
        }
        else
        {
            if(SpeedRegs.count_w1>1 && SpeedRegs.count_w1<(SpeedRegs.spd_down_step2+2) && (S_remnant_down >0))  //按指数规律计算速度
            {
                SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (SpeedRegs.count_w1-2)*SpeedRegs.JJSpeed;
                SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi - 1;
                S_remnant_down = S_remnant_down - 1;
            }
            else if(SpeedRegs.count_w1>1 && SpeedRegs.count_w1<(SpeedRegs.spd_down_step2+2))
                SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (SpeedRegs.count_w1-2)*SpeedRegs.JJSpeed;
            if((SpeedRegs.spd_down_step3!=0) && ((SpeedRegs.spd_down_step2+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+2)) && (S_remnant_down >0))
            {
                SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (SpeedRegs.spd_down_step2-1)*SpeedRegs.JJSpeed;
                SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi - 1;
                S_remnant_down = S_remnant_down - 1;
            }
            else if((SpeedRegs.spd_down_step3!=0) && ((SpeedRegs.spd_down_step2+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+2)))
                SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (SpeedRegs.spd_down_step2-1)*SpeedRegs.JJSpeed;
            if(((SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+2)) && (S_remnant_down >0))
            {
                SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+1-SpeedRegs.count_w1)*SpeedRegs.JJSpeed;
                SpeedRegs.sect_in.vi = SpeedRegs.sect_in.vi - 1;
                S_remnant_down = S_remnant_down - 1;
            }
            else if (((SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+1)<SpeedRegs.count_w1) && (SpeedRegs.count_w1<(2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+2)))
                SpeedRegs.sect_in.vi=SpeedRegs.sect_in.vi + SpeedRegs.a_ini_down - (2*SpeedRegs.spd_down_step2+SpeedRegs.spd_down_step3+1-SpeedRegs.count_w1)*SpeedRegs.JJSpeed;
        }
    }
    if (SpeedRegs.adjust_feed_flag==3)  // 将减速段补给加速段
    {
        if (SpeedRegs.delta_feed_cnt>=1)
        {
            SpeedRegs.sect_in_feed_BK = SpeedRegs.sect_in.vi + SpeedRegs.adjust_feed+1;
            SpeedRegs.delta_feed_cnt--;
        }//chenlin2014.6.5beifen
        else if(SpeedRegs.delta_feed_cnt<0)
        {
            SpeedRegs.sect_in_feed_BK=SpeedRegs.sect_in.vi + SpeedRegs.adjust_feed-1;
            SpeedRegs.delta_feed_cnt++;
        }
        else//chenlin2014.6.5beifen
            SpeedRegs.sect_in_feed_BK = SpeedRegs.sect_in.vi + SpeedRegs.adjust_feed;
    }
    else
        SpeedRegs.sect_in_feed_BK= SpeedRegs.sect_in.vi;
}

// ---------------------------------------------------------------------------
// 函数功能：   加减速控制
// 输入参数：   无
// 输出参数：   当前段速度
// 全局变量:     当前段坐标, 当前段速度, 当前段加工类型,
//                   上一段的fc, 下一段坐标, 下一段速度
//                   下一段加工类型, 下下一段坐标
// 创建时间：   2006.07.14
// 最后修改时间:2011
// 修改原因：   改为S加减速
// 创建人  ：   付科
// ---------------------------------------------------------------------------
void speed_control(void)
{
    switch (SpeedRegs.updown_stats)
    {
    case 0:                 // 计算末端速度、加速段长、减速段长
        Speed_Control0();
        break;
    case 1:                  // 加速段
        Speed_Control1();
        break;
    case 3:                  // 匀速段
        Speed_Control3();
        break;
    case 5:                  // 减速段
        Speed_Control5();
        break;
    case 6:                   // 该段完成
        break;
    }
    if(SpeedRegs.sect_in_feed_BK < SpeedRegs.speed_min)
        SpeedRegs.sect_in_feed_BK = SpeedRegs.speed_min;
    if((EncodeRegs.AsReg.Type == 2 || EncodeRegs.AsReg.Type ==3)&&RunstatusRegs.circle_MDIstop_flag >= 2 && SpeedRegs.sect_in_feed_BK >= SpeedRegs.sect_in_feed_BK1)//chenlin2014.3.13
        RunstatusRegs.circle_MDIstop_flag ++;
    SpeedRegs.rem_length = SpeedRegs.rem_length - SpeedRegs.sect_in_feed_BK;
    SpeedRegs.sect_in_feed_BK1 = SpeedRegs.sect_in_feed_BK;//chenlin2014.3.13
    if((SpeedRegs.rem_length>0 && SpeedRegs.rem_length<10&&SpeedRegs.sect_in_feed_BK>=30))
    {
        SpeedRegs.sect_in_feed_BK=SpeedRegs.sect_in_feed_BK+SpeedRegs.rem_length;
        SpeedRegs.rem_length = 0;
    }
    if(SpeedRegs.rem_length <= 0)
    {
        SpeedRegs.updown_stats = 6;             // 该段完成
        SpeedRegs.sect_in_feed_BK=SpeedRegs.rem_length+SpeedRegs.sect_in_feed_BK;
        if(SpeedRegs.sect_in_feed_BK < SpeedRegs.speed_min)  // 防止插补速度为0的情况
            SpeedRegs.sect_in_feed_BK = SpeedRegs.speed_min;
        SpeedRegs.adjust_feed_flag=0;
        SpeedRegs.rem_length = 0;
        SpeedRegs.sect_Lin.S = SpeedRegs.sect_in.S;
        SpeedRegs.sect_Lin.fc = SpeedRegs.sect_in.fc;
        SpeedRegs.sect_Lin.length = SpeedRegs.sect_in.length;
    }
}
// 前加减速处理
void speed_pre(void)
{
    SpeedRegs.sect_in.feed = EncodeRegs.AsReg.F;            // 当前段速度
    SpeedRegs.sect_in.end.pos_x = EncodeRegs.AsReg.X;
    SpeedRegs.sect_in.end.pos_y = EncodeRegs.AsReg.Y;
    SpeedRegs.sect_in.end.pos_z = EncodeRegs.AsReg.Z;

    SpeedRegs.sect_in.end.pos_a = EncodeRegs.AsReg.A;
    SpeedRegs.sect_in.end.pos_b = EncodeRegs.AsReg.B;
    SpeedRegs.sect_in.end.pos_c = EncodeRegs.AsReg.C;//chenlin2014.3.8

/*  SpeedRegs.sect_in.end.pos_a = EncodeRegs.AsReg.Alfa;
    SpeedRegs.sect_in.end.pos_b = EncodeRegs.AsReg.Gama;
    SpeedRegs.sect_in.end.pos_c = EncodeRegs.AsReg.Beta;*///chenlin2014.3.8
    if(EncodeRegs.AsReg.Type == 100)
    {
        SpeedRegs.sect_in.end.pos_x = EncodeRegs.AsReg.D;       // 陈林2013.12.5
        SpeedRegs.sect_in.end.pos_y = EncodeRegs.AsReg.E;
        SpeedRegs.sect_in.end.pos_z = EncodeRegs.AsReg.N;
        SpeedRegs.sect_in.end.pos_a = EncodeRegs.AsReg.O;
        SpeedRegs.sect_in.end.pos_b = EncodeRegs.AsReg.Q;
        SpeedRegs.sect_in.end.pos_c = EncodeRegs.AsReg.V;
    }
    SpeedRegs.sect_in.plane = EncodeRegs.AsReg.Plate;
    SpeedRegs.sect_in.S = EncodeRegs.AsReg.S;
    SpeedRegs.sect_in.i = EncodeRegs.AsReg.I;
    SpeedRegs.sect_in.j = EncodeRegs.AsReg.J;
    SpeedRegs.sect_in.k = EncodeRegs.AsReg.K;
    SpeedRegs.sect_in.R = CirCleInterRegs.st_end_cir.cir_r;

    SpeedRegs.sect_Nin.type = EncodeRegs.BsReg.GX2;         // 下一段加工类型
    if(EncodeRegs.BsReg.M == 30 || EncodeRegs.BsReg.M1 == 30)    //chenlin2013.12.13
        SpeedRegs.sect_Nin.type = -2;
    SpeedRegs.sect_Nin.feed = EncodeRegs.BsReg.F; //EncodeRegs.BsReg.F/15;        // 下一段速度
//    SpeedRegs.sect_Nin.feed =SpeedRegs.sect_Nin.feed *  MainAxisRegs.S_BeiLv / 100;
//    if(SpeedRegs.sect_Nin.feed == 0 && EncodeRegs.BsReg.F != 0)
//        SpeedRegs.sect_Nin.feed = 1;
    SpeedRegs.sect_Nin.st = SpeedRegs.sect_in.end;          // 下一段坐标
    SpeedRegs.sect_Nin.end.pos_x = EncodeRegs.BsReg.X;      // 下下一段坐标
    SpeedRegs.sect_Nin.end.pos_y = EncodeRegs.BsReg.Y;
    SpeedRegs.sect_Nin.end.pos_z = EncodeRegs.BsReg.Z;

    SpeedRegs.sect_Nin.end.pos_a = EncodeRegs.BsReg.A;
    SpeedRegs.sect_Nin.end.pos_b = EncodeRegs.BsReg.B;
    SpeedRegs.sect_Nin.end.pos_c = EncodeRegs.BsReg.C;            //chenlin2014.3.8
    if(EncodeRegs.BsReg.GX2 == 100)
    {
        SpeedRegs.sect_Nin.end.pos_x = EncodeRegs.BsReg.D;      // 陈林2013.12.5
        SpeedRegs.sect_Nin.end.pos_y = EncodeRegs.BsReg.E;
        SpeedRegs.sect_Nin.end.pos_z = EncodeRegs.BsReg.N;
        SpeedRegs.sect_Nin.end.pos_a = EncodeRegs.BsReg.O;
        SpeedRegs.sect_Nin.end.pos_b = EncodeRegs.BsReg.Q;
        SpeedRegs.sect_Nin.end.pos_c = EncodeRegs.BsReg.V;
    }
    SpeedRegs.sect_Nin.plane = EncodeRegs.BsReg.GX7;
    SpeedRegs.sect_Nin.i = EncodeRegs.BsReg.I;
    SpeedRegs.sect_Nin.j = EncodeRegs.BsReg.J;
    SpeedRegs.sect_Nin.k = EncodeRegs.BsReg.K;
    SpeedRegs.sect_Nin.R = EncodeRegs.BsReg.R;
    SpeedRegs.sect_Nin.S = EncodeRegs.BsReg.S;
}
// 进给倍率信号
void Speed_BeiLv()
{
    uint32_t UniVelo_X,UniVelo_Y,UniVelo_Z,UniVelo_A,UniVelo_B,UniVelo_C,UniVelo_W,UniVelo_U;
//  MainAxisRegs.S_BeiLv = InputE & 0x00FF;//控制软件下传倍率信号InputE
    switch(MainAxisRegs.S_BeiLv)
    {
        case 0x0000:
            MainAxisRegs.S_BeiLv = 0;
            break;
        case 0x0010:
            MainAxisRegs.S_BeiLv = 10;
            break;
        case 0x0020:
            MainAxisRegs.S_BeiLv = 30;
            break;
        case 0x0030:
            MainAxisRegs.S_BeiLv = 60;
            break;
        case 0x0040:
            MainAxisRegs.S_BeiLv = 80;
            break;
        case 0x0050:
            MainAxisRegs.S_BeiLv = 100;
            break;
        case 0x0060:
            MainAxisRegs.S_BeiLv = 110;
            break;
        case 0x0070:
            MainAxisRegs.S_BeiLv = 120;
            break;
    }
// 读取快速进给倍率  JOG
    UniVelo_X = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[0] / 15;    // 快移速度 *((uint16_t *)0x8002C)
    UniVelo_Y = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[1] / 15;
    UniVelo_Z = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[2] / 15;
    UniVelo_A = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[3] / 15;
    UniVelo_B = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[4] / 15;
    UniVelo_C = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[5] / 15;
    UniVelo_W = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[6] / 15;
    UniVelo_U = ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[7] / 15;
// 速度倍率调整
    UniVelo_X = UniVelo_X * MainAxisRegs.S_BeiLv / 100;
    UniVelo_Y = UniVelo_Y * MainAxisRegs.S_BeiLv / 100;
    UniVelo_Z = UniVelo_Z * MainAxisRegs.S_BeiLv / 100;
    UniVelo_A = UniVelo_A * MainAxisRegs.S_BeiLv / 100;
    UniVelo_B = UniVelo_B * MainAxisRegs.S_BeiLv / 100;
    UniVelo_C = UniVelo_C * MainAxisRegs.S_BeiLv / 100;
    UniVelo_W = UniVelo_W * MainAxisRegs.S_BeiLv / 100;
    UniVelo_U = UniVelo_U * MainAxisRegs.S_BeiLv / 100;
    if(UniVelo_X == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[0])
        UniVelo_X = 1;
    if(UniVelo_Y == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[1])
        UniVelo_Y = 1;
    if(UniVelo_Z == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[2])
        UniVelo_Z = 1;
    if(UniVelo_A == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[3])
        UniVelo_A = 1;
    if(UniVelo_B == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[4])
        UniVelo_B = 1;
    if(UniVelo_C == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[5])
        UniVelo_C = 1;
    if(UniVelo_W == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[6])
        UniVelo_W = 1;
    if(UniVelo_U == 0 && ParamterUnRegs.ParamterRegs.AxisparmRegs.FastMoveSpeed[7])
        UniVelo_U = 1;
    RPDAccDecRegs.UniVelo_X = UniVelo_X;
    RPDAccDecRegs.UniVelo_Y = UniVelo_Y;
    RPDAccDecRegs.UniVelo_Z = UniVelo_Z;
    RPDAccDecRegs.UniVelo_A = UniVelo_A;
    RPDAccDecRegs.UniVelo_B = UniVelo_B;
    RPDAccDecRegs.UniVelo_C = UniVelo_C;
    RPDAccDecRegs.UniVelo_W = UniVelo_W;
    RPDAccDecRegs.UniVelo_U = UniVelo_U;
}
// ---------------------------------------------------------------------------
// 函数功能：   快速定位
// 输入参数：   无..
// 输出参数：   无
// 创建时间：   2006.03.24
// 最后修改时间:2006.07.20
// 修改原因：   添加前加减速
// 创建人  ：   杨晓龙
// ---------------------------------------------------------------------------

//************************************************************************************
void RpdRun_old(void)
{
    int32_t tempvelo_x, tempvelo_y, tempvelo_z, tempvelo_a, tempvelo_b, tempvelo_c;
    int32_t temp;
    // 进给倍率调整
    tempvelo_x = RPDAccDecRegs.UniVelo_X * MainAxisRegs.S_BeiLv / 100;
    tempvelo_y = RPDAccDecRegs.UniVelo_Y * MainAxisRegs.S_BeiLv / 100;
    tempvelo_z = RPDAccDecRegs.UniVelo_Z * MainAxisRegs.S_BeiLv / 100;
    tempvelo_a = RPDAccDecRegs.UniVelo_A * MainAxisRegs.S_BeiLv / 100;
    tempvelo_b = RPDAccDecRegs.UniVelo_B * MainAxisRegs.S_BeiLv / 100;
    tempvelo_c = RPDAccDecRegs.UniVelo_C * MainAxisRegs.S_BeiLv / 100;
    if((RunstatusRegs.RpdFlag == 0) && ((EncodeRegs.AsReg.X != PostionRegs.real_pos.pos_x) ||
                            (EncodeRegs.AsReg.Z != PostionRegs.real_pos.pos_y) ||
                            (EncodeRegs.AsReg.Z != PostionRegs.real_pos.pos_z) ||
                            (EncodeRegs.AsReg.A != PostionRegs.real_pos.pos_a) ||
                            (EncodeRegs.AsReg.B != PostionRegs.real_pos.pos_b) ||
                            (EncodeRegs.AsReg.C != PostionRegs.real_pos.pos_c)))
    {
        RunstatusRegs.RpdFlag = 63;         // 快速定位初始化
        RPDAccDecRegs.RPD_vx = 0;
        RPDAccDecRegs.RPD_vy = 0;
        RPDAccDecRegs.RPD_vz = 0;
        RPDAccDecRegs.RPD_va = 0;
        RPDAccDecRegs.RPD_vb = 0;
        RPDAccDecRegs.RPD_vc = 0;

        if(RPDAccDecRegs.spd_down_par1 == 0)        // 无加减速
        {
            RPDAccDecRegs.spd_down_velo1 = 0x7FFF;
            RPDAccDecRegs.RPD_ud_lengthx = 0;
        }
        else                        // 加减速计算
        {
            RPDAccDecRegs.spd_down_velo1 = tempvelo_x / (int32_t)RPDAccDecRegs.spd_down_par1;
            if(RPDAccDecRegs.spd_down_velo1 == 0 && tempvelo_x != 0)
                RPDAccDecRegs.spd_down_velo1 = 1;
            RPDAccDecRegs.RPD_ud_lengthx = tempvelo_x / RPDAccDecRegs.spd_down_velo1 / 2 + tempvelo_x * tempvelo_x / RPDAccDecRegs.spd_down_velo1 / 2;
        }

        if(RPDAccDecRegs.spd_down_par2 == 0)
        {
            RPDAccDecRegs.spd_down_velo2 = 0x7FFF;
            RPDAccDecRegs.RPD_ud_lengthy = 0;
        }
        else
        {
            RPDAccDecRegs.spd_down_velo2 = tempvelo_y / (int32_t)RPDAccDecRegs.spd_down_par2;
            if(RPDAccDecRegs.spd_down_velo2 == 0 && tempvelo_y != 0)
                RPDAccDecRegs.spd_down_velo2 = 1;
            RPDAccDecRegs.RPD_ud_lengthy = tempvelo_y / RPDAccDecRegs.spd_down_velo2 / 2 + tempvelo_y * tempvelo_y / RPDAccDecRegs.spd_down_velo2 / 2;
        }

        if(RPDAccDecRegs.spd_down_par3 == 0)
        {
            RPDAccDecRegs.spd_down_velo3 = 0x7FFF;
            RPDAccDecRegs.RPD_ud_lengthz = 0;
        }
        else
        {
            RPDAccDecRegs.spd_down_velo3 = tempvelo_z / (int32_t)RPDAccDecRegs.spd_down_par3;
            if(RPDAccDecRegs.spd_down_velo3 == 0 && tempvelo_z != 0)
                RPDAccDecRegs.spd_down_velo3 = 1;
            RPDAccDecRegs.RPD_ud_lengthz = tempvelo_z / RPDAccDecRegs.spd_down_velo3 / 2 + tempvelo_z * tempvelo_z / RPDAccDecRegs.spd_down_velo3 / 2;
        }

        if(RPDAccDecRegs.spd_down_par4 == 0)
        {
            RPDAccDecRegs.spd_down_velo4 = 0x7FFF;
            RPDAccDecRegs.RPD_ud_lengtha = 0;
        }
        else
        {
            RPDAccDecRegs.spd_down_velo4 = tempvelo_a / (int32_t)RPDAccDecRegs.spd_down_par4;
            if(RPDAccDecRegs.spd_down_velo4 == 0 && tempvelo_a != 0)
                RPDAccDecRegs.spd_down_velo4 = 1;
            RPDAccDecRegs.RPD_ud_lengtha = tempvelo_a / RPDAccDecRegs.spd_down_velo4 / 2 + tempvelo_a * tempvelo_a / RPDAccDecRegs.spd_down_velo4 / 2;
        }

        if(RPDAccDecRegs.spd_down_par5 == 0)
        {
            RPDAccDecRegs.spd_down_velo5 = 0x7FFF;
            RPDAccDecRegs.RPD_ud_lengthb = 0;
        }
        else
        {
            RPDAccDecRegs.spd_down_velo5 = tempvelo_b / (int32_t)RPDAccDecRegs.spd_down_par5;
            if(RPDAccDecRegs.spd_down_velo5 == 0 && tempvelo_b != 0)
                RPDAccDecRegs.spd_down_velo5 = 1;
            RPDAccDecRegs.RPD_ud_lengthb = tempvelo_b / RPDAccDecRegs.spd_down_velo5 / 2 + tempvelo_b * tempvelo_b / RPDAccDecRegs.spd_down_velo5 / 2;
        }

        if(RPDAccDecRegs.spd_down_par6 == 0)
        {
            RPDAccDecRegs.spd_down_velo6 = 0x7FFF;
            RPDAccDecRegs.RPD_ud_lengthc = 0;
        }
        else
        {
            RPDAccDecRegs.spd_down_velo6 = tempvelo_c / (int32_t)RPDAccDecRegs.spd_down_par6;
            if(RPDAccDecRegs.spd_down_velo6 == 0 && tempvelo_c != 0)
                RPDAccDecRegs.spd_down_velo6 = 1;
            RPDAccDecRegs.RPD_ud_lengthc = tempvelo_c / RPDAccDecRegs.spd_down_velo6 / 2 + tempvelo_c * tempvelo_c / RPDAccDecRegs.spd_down_velo6 / 2;
        }

        temp = labs(PostionRegs.real_pos.pos_x - EncodeRegs.AsReg.X);
        if((RPDAccDecRegs.RPD_ud_lengthx + RPDAccDecRegs.RPD_ud_lengthx) > temp)
            RPDAccDecRegs.RPD_ud_lengthx = temp / 2;

        temp = labs(PostionRegs.real_pos.pos_y - EncodeRegs.AsReg.Y);
        if((RPDAccDecRegs.RPD_ud_lengthy + RPDAccDecRegs.RPD_ud_lengthy) > temp)
            RPDAccDecRegs.RPD_ud_lengthy = temp / 2;

        temp = labs(PostionRegs.real_pos.pos_z - EncodeRegs.AsReg.Z);
        if((RPDAccDecRegs.RPD_ud_lengthz + RPDAccDecRegs.RPD_ud_lengthz) > temp)
            RPDAccDecRegs.RPD_ud_lengthz = temp / 2;

        temp = labs(PostionRegs.real_pos.pos_a - EncodeRegs.AsReg.A);
        if((RPDAccDecRegs.RPD_ud_lengtha + RPDAccDecRegs.RPD_ud_lengtha) > temp)
            RPDAccDecRegs.RPD_ud_lengtha = temp / 2;

        temp = labs(PostionRegs.real_pos.pos_b - EncodeRegs.AsReg.B);
        if((RPDAccDecRegs.RPD_ud_lengthb + RPDAccDecRegs.RPD_ud_lengthb) > temp)
            RPDAccDecRegs.RPD_ud_lengthb = temp / 2;

        temp = labs(PostionRegs.real_pos.pos_c - EncodeRegs.AsReg.C);
        if((RPDAccDecRegs.RPD_ud_lengthc + RPDAccDecRegs.RPD_ud_lengthc) > temp)
            RPDAccDecRegs.RPD_ud_lengthc = temp / 2;
    }
    if(PostionRegs.real_pos.pos_x < (EncodeRegs.AsReg.X - RPDAccDecRegs.RPD_vx))                                    // X轴正方向运动
    {
        if(PostionRegs.real_pos.pos_x >= (EncodeRegs.AsReg.X - RPDAccDecRegs.RPD_ud_lengthx))       // 段尾减速
        {
            if(RPDAccDecRegs.RPD_vx < RPDAccDecRegs.spd_down_velo1 + RPDAccDecRegs.spd_down_velo1)
                RPDAccDecRegs.RPD_vx = RPDAccDecRegs.spd_down_velo1;
            else
                RPDAccDecRegs.RPD_vx -= RPDAccDecRegs.spd_down_velo1;
        }                                                   // 段末尾处理
        else if(RPDAccDecRegs.RPD_vx < (tempvelo_x - RPDAccDecRegs.spd_down_velo1))     // 段首加速
            RPDAccDecRegs.RPD_vx += RPDAccDecRegs.spd_down_velo1;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vx = tempvelo_x;

        if(RPDAccDecRegs.RPD_vx <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vx = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_x += RPDAccDecRegs.RPD_vx;
    }
    else if(PostionRegs.real_pos.pos_x > (EncodeRegs.AsReg.X + RPDAccDecRegs.RPD_vx))                       // X轴负方向运动
    {
        if(PostionRegs.real_pos.pos_x <= (EncodeRegs.AsReg.X + (int32_t)RPDAccDecRegs.RPD_ud_lengthx))            // 段尾减速
        {
            if(RPDAccDecRegs.RPD_vx < RPDAccDecRegs.spd_down_velo1 + RPDAccDecRegs.spd_down_velo1)
                RPDAccDecRegs.RPD_vx = RPDAccDecRegs.spd_down_velo1;
            else
                RPDAccDecRegs.RPD_vx -= RPDAccDecRegs.spd_down_velo1;
        }
        else if(RPDAccDecRegs.RPD_vx < (tempvelo_x - RPDAccDecRegs.spd_down_velo1))     // 段首加速
            RPDAccDecRegs.RPD_vx += RPDAccDecRegs.spd_down_velo1;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vx = tempvelo_x;

        if(RPDAccDecRegs.RPD_vx <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vx = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_x -= RPDAccDecRegs.RPD_vx;
    }
    else                // 末尾处理
    {
        PostionRegs.real_pos.pos_x = EncodeRegs.AsReg.X;
        RPDAccDecRegs.RPD_vx = 0;
    }


    if(PostionRegs.real_pos.pos_y < (EncodeRegs.AsReg.Y - RPDAccDecRegs.RPD_vy))                            // Y轴正方向运动
    {
        if(PostionRegs.real_pos.pos_y >= (EncodeRegs.AsReg.Y - RPDAccDecRegs.RPD_ud_lengthy))   // 段尾减速
        {
            if(RPDAccDecRegs.RPD_vy < RPDAccDecRegs.spd_down_velo2 + RPDAccDecRegs.spd_down_velo2)
                RPDAccDecRegs.RPD_vy = RPDAccDecRegs.spd_down_velo2;
            else
                RPDAccDecRegs.RPD_vy -= RPDAccDecRegs.spd_down_velo2;
        }                                                   // 段末尾处理
        else if(RPDAccDecRegs.RPD_vy < (tempvelo_y - RPDAccDecRegs.spd_down_velo2))     // 段首加速
            RPDAccDecRegs.RPD_vy += RPDAccDecRegs.spd_down_velo2;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vy = tempvelo_y;

        if(RPDAccDecRegs.RPD_vy <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vy = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_y += RPDAccDecRegs.RPD_vy;
    }
    else if(PostionRegs.real_pos.pos_y > (EncodeRegs.AsReg.Y + RPDAccDecRegs.RPD_vy))                       // Y轴负方向运动
    {
        if(PostionRegs.real_pos.pos_y <= (EncodeRegs.AsReg.Y + RPDAccDecRegs.RPD_ud_lengthy))   // 段尾减速
        {
            if(RPDAccDecRegs.RPD_vy < RPDAccDecRegs.spd_down_velo2 + RPDAccDecRegs.spd_down_velo2)
                RPDAccDecRegs.RPD_vy = RPDAccDecRegs.spd_down_velo2;
            else
                RPDAccDecRegs.RPD_vy -= RPDAccDecRegs.spd_down_velo2;
        }
        else if(RPDAccDecRegs.RPD_vy < (tempvelo_y - RPDAccDecRegs.spd_down_velo2))     // 段首加速
            RPDAccDecRegs.RPD_vy += RPDAccDecRegs.spd_down_velo2;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vy = tempvelo_y;

        if(RPDAccDecRegs.RPD_vy <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vy = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_y -= RPDAccDecRegs.RPD_vy;
    }
    else
    {
        PostionRegs.real_pos.pos_y = EncodeRegs.AsReg.Y;
        RPDAccDecRegs.RPD_vy = 0;
    }

     if(PostionRegs.real_pos.pos_z < (EncodeRegs.AsReg.Z - RPDAccDecRegs.RPD_vz))                                   // Z轴正方向运动
    {
        if(PostionRegs.real_pos.pos_z >= (EncodeRegs.AsReg.Z - RPDAccDecRegs.RPD_ud_lengthz))       // 段尾减速
        {
            if(RPDAccDecRegs.RPD_vz < RPDAccDecRegs.spd_down_velo3 + RPDAccDecRegs.spd_down_velo3)
                RPDAccDecRegs.RPD_vz = RPDAccDecRegs.spd_down_velo3;
            else
                RPDAccDecRegs.RPD_vz -= RPDAccDecRegs.spd_down_velo3;
        }                                                   // 段末尾处理
        else if(RPDAccDecRegs.RPD_vz < (tempvelo_z - RPDAccDecRegs.spd_down_velo3))     // 段首加速
            RPDAccDecRegs.RPD_vz += RPDAccDecRegs.spd_down_velo3;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vz = tempvelo_z;

        if(RPDAccDecRegs.RPD_vz <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vz = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_z += RPDAccDecRegs.RPD_vz;
    }
    else if(PostionRegs.real_pos.pos_z > (EncodeRegs.AsReg.Z + RPDAccDecRegs.RPD_vz))                       // Z轴负方向运动
    {
        if(PostionRegs.real_pos.pos_z <= (EncodeRegs.AsReg.Z + RPDAccDecRegs.RPD_ud_lengthz))
        {
            if(RPDAccDecRegs.RPD_vz < RPDAccDecRegs.spd_down_velo3 + RPDAccDecRegs.spd_down_velo3)
                RPDAccDecRegs.RPD_vz = RPDAccDecRegs.spd_down_velo3;
            else
                RPDAccDecRegs.RPD_vz -= RPDAccDecRegs.spd_down_velo3;
        }
        else if(RPDAccDecRegs.RPD_vz < (tempvelo_z - RPDAccDecRegs.spd_down_velo3))     // 段首加速
            RPDAccDecRegs.RPD_vz += RPDAccDecRegs.spd_down_velo3;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vz = tempvelo_z;

        if(RPDAccDecRegs.RPD_vz <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vz = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_z -= RPDAccDecRegs.RPD_vz;
    }
    else
    {
        PostionRegs.real_pos.pos_z = EncodeRegs.AsReg.Z;
        RPDAccDecRegs.RPD_vz = 0;
    }


    if(PostionRegs.real_pos.pos_a < (EncodeRegs.AsReg.A - RPDAccDecRegs.RPD_va))                                    // A轴正方向运动
    {
        if(PostionRegs.real_pos.pos_a >= (EncodeRegs.AsReg.A - RPDAccDecRegs.RPD_ud_lengtha))       // 段尾减速
        {
            if(RPDAccDecRegs.RPD_va < RPDAccDecRegs.spd_down_velo4 + RPDAccDecRegs.spd_down_velo4)
                RPDAccDecRegs.RPD_va = RPDAccDecRegs.spd_down_velo4;
            else
                RPDAccDecRegs.RPD_va -= RPDAccDecRegs.spd_down_velo4;
        }                                                   // 段末尾处理
        else if(RPDAccDecRegs.RPD_va < (tempvelo_a - RPDAccDecRegs.spd_down_velo4))     // 段首加速
            RPDAccDecRegs.RPD_va += RPDAccDecRegs.spd_down_velo4;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_va = tempvelo_a;

        if(RPDAccDecRegs.RPD_va <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_va = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_a += RPDAccDecRegs.RPD_va;
    }
    else if(PostionRegs.real_pos.pos_a > (EncodeRegs.AsReg.A + RPDAccDecRegs.RPD_va))                       // A轴负方向运动
    {
        if(PostionRegs.real_pos.pos_a <= (EncodeRegs.AsReg.A + RPDAccDecRegs.RPD_ud_lengtha))
        {
            if(RPDAccDecRegs.RPD_va < RPDAccDecRegs.spd_down_velo4 + RPDAccDecRegs.spd_down_velo4)
                RPDAccDecRegs.RPD_va = RPDAccDecRegs.spd_down_velo4;
            else
                RPDAccDecRegs.RPD_va -= RPDAccDecRegs.spd_down_velo4;
        }
        else if(RPDAccDecRegs.RPD_va < (tempvelo_a - RPDAccDecRegs.spd_down_velo4))     // 段首加速
            RPDAccDecRegs.RPD_va += RPDAccDecRegs.spd_down_velo4;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_va = tempvelo_a;

        if(RPDAccDecRegs.RPD_va <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_va = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_a -= RPDAccDecRegs.RPD_va;
    }
    else
    {
        PostionRegs.real_pos.pos_a = EncodeRegs.AsReg.A;
        RPDAccDecRegs.RPD_va = 0;
    }


    if(PostionRegs.real_pos.pos_b < (EncodeRegs.AsReg.B - RPDAccDecRegs.RPD_vb))                                    // B轴正方向运动
    {
        if(PostionRegs.real_pos.pos_b >= (EncodeRegs.AsReg.B - RPDAccDecRegs.RPD_ud_lengthb))       // 段尾减速
        {
            if(RPDAccDecRegs.RPD_vb < RPDAccDecRegs.spd_down_velo5 + RPDAccDecRegs.spd_down_velo5)
                RPDAccDecRegs.RPD_vb = RPDAccDecRegs.spd_down_velo5;
            else
                RPDAccDecRegs.RPD_vb -= RPDAccDecRegs.spd_down_velo5;
        }                                                   // 段末尾处理
        else if(RPDAccDecRegs.RPD_vb < (tempvelo_b - RPDAccDecRegs.spd_down_velo5))     // 段首加速
            RPDAccDecRegs.RPD_vb += RPDAccDecRegs.spd_down_velo5;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vb = tempvelo_b;

        if(RPDAccDecRegs.RPD_vb <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vb = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_b += RPDAccDecRegs.RPD_vb;
    }
    else if(PostionRegs.real_pos.pos_b > (EncodeRegs.AsReg.B + RPDAccDecRegs.RPD_vb))                       // B轴负方向运动
    {
        if(PostionRegs.real_pos.pos_b <= (EncodeRegs.AsReg.B + RPDAccDecRegs.RPD_ud_lengthb))
        {
            if(RPDAccDecRegs.RPD_vb < RPDAccDecRegs.spd_down_velo5 + RPDAccDecRegs.spd_down_velo5)
                RPDAccDecRegs.RPD_vb = RPDAccDecRegs.spd_down_velo5;
            else
                RPDAccDecRegs.RPD_vb -= RPDAccDecRegs.spd_down_velo5;
        }
        else if(RPDAccDecRegs.RPD_vb < (tempvelo_b - RPDAccDecRegs.spd_down_velo5))     // 段首加速
            RPDAccDecRegs.RPD_vb += RPDAccDecRegs.spd_down_velo5;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vb = tempvelo_b;

        if(RPDAccDecRegs.RPD_vb <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vb = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_b -= RPDAccDecRegs.RPD_vb;
    }
    else
    {
        PostionRegs.real_pos.pos_b = EncodeRegs.AsReg.B;
        RPDAccDecRegs.RPD_vb = 0;
    }
    if(PostionRegs.real_pos.pos_c < (EncodeRegs.AsReg.C - RPDAccDecRegs.RPD_vc))                                    // C轴正方向运动
    {
        if(PostionRegs.real_pos.pos_c >= (EncodeRegs.AsReg.C - RPDAccDecRegs.RPD_ud_lengthc))       // 段尾减速
        {
            if(RPDAccDecRegs.RPD_vc < RPDAccDecRegs.spd_down_velo6 + RPDAccDecRegs.spd_down_velo6)
                RPDAccDecRegs.RPD_vc = RPDAccDecRegs.spd_down_velo6;
            else
                RPDAccDecRegs.RPD_vc -= RPDAccDecRegs.spd_down_velo6;
        }                                                   // 段末尾处理
        else if(RPDAccDecRegs.RPD_vc < (tempvelo_c - RPDAccDecRegs.spd_down_velo6))     // 段首加速
            RPDAccDecRegs.RPD_vc += RPDAccDecRegs.spd_down_velo6;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vc = tempvelo_c;

        if(RPDAccDecRegs.RPD_vc <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vc = SpeedRegs.speed_min;

        PostionRegs.real_pos.pos_c += RPDAccDecRegs.RPD_vc;
    }
    else if(PostionRegs.real_pos.pos_c > (EncodeRegs.AsReg.C + RPDAccDecRegs.RPD_vc))                       // C轴负方向运动
    {
        if(PostionRegs.real_pos.pos_c <= (EncodeRegs.AsReg.C + RPDAccDecRegs.RPD_ud_lengthc))
        {
            if(RPDAccDecRegs.RPD_vc < RPDAccDecRegs.spd_down_velo6 + RPDAccDecRegs.spd_down_velo6)
                RPDAccDecRegs.RPD_vc = RPDAccDecRegs.spd_down_velo6;
            else
                RPDAccDecRegs.RPD_vc -= RPDAccDecRegs.spd_down_velo6;
        }
        else if(RPDAccDecRegs.RPD_vc < (tempvelo_c - RPDAccDecRegs.spd_down_velo6))     // 段首加速
            RPDAccDecRegs.RPD_vc += RPDAccDecRegs.spd_down_velo6;
        else                                                // 段中匀速
            RPDAccDecRegs.RPD_vc = tempvelo_c;
        if(RPDAccDecRegs.RPD_vc <= SpeedRegs.speed_min)
            RPDAccDecRegs.RPD_vc = SpeedRegs.speed_min;
        PostionRegs.real_pos.pos_c -= RPDAccDecRegs.RPD_vc;
    }
    else
    {
        PostionRegs.real_pos.pos_c = EncodeRegs.AsReg.C;
        RPDAccDecRegs.RPD_vc = 0;
    }

    if((EncodeRegs.AsReg.X==PostionRegs.real_pos.pos_x)&&(EncodeRegs.AsReg.Y==PostionRegs.real_pos.pos_y)&&
       (EncodeRegs.AsReg.Z==PostionRegs.real_pos.pos_z)&&(EncodeRegs.AsReg.A==PostionRegs.real_pos.pos_a)&&
       (EncodeRegs.AsReg.B==PostionRegs.real_pos.pos_b)&&(EncodeRegs.AsReg.C==PostionRegs.real_pos.pos_c))
    {
        if(EncodeRegs.AsReg.GX1==28)                // G28先快速定位到指定的坐标,然后返回到坐标原点
        {
            if(RunstatusRegs.G28Flag==0)
                RunstatusRegs.G28Flag++;
            else
            {
 //             DioRegs.LapEn1 = 0x0000;                        // 一转信号不使能
                RunstatusRegs.G28Flag = 0;
                EncodeRegs.AsReg.GX1 = 0;
                RunstatusRegs.ret_finished_x = -1;
                RunstatusRegs.ret_finished_y = -1;
                RunstatusRegs.ret_finished_z = -1;
                RunstatusRegs.ret_finished_a = -1;
                RunstatusRegs.ret_finished_b = -1;
                RunstatusRegs.ret_finished_c = -1;
                if(PostionRegs.real_pos.pos_x < (ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[0] + 10000 + RunstatusRegs.ret_offset))   // 回退坐标
                    MainAxisRegs.dir_comp.x = -1;
                else
                    MainAxisRegs.dir_comp.x = 1;
                if(PostionRegs.real_pos.pos_y < (ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[1] + 10000 + RunstatusRegs.ret_offset))
                MainAxisRegs.dir_comp.y = -1;
                else
                MainAxisRegs.dir_comp.y = 1;
                if(PostionRegs.real_pos.pos_z < (ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[2] + 10000 + RunstatusRegs.ret_offset))
                    MainAxisRegs.dir_comp.z = -1;
                else
                    MainAxisRegs.dir_comp.z = 1;
                if(PostionRegs.real_pos.pos_a < (ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[3] + 10000 + RunstatusRegs.ret_offset))
                    MainAxisRegs.dir_comp.a = -1;
                else
                    MainAxisRegs.dir_comp.a = 1;
                if(PostionRegs.real_pos.pos_b < (ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[4] + 10000 + RunstatusRegs.ret_offset))
                    MainAxisRegs.dir_comp.b = -1;
                else
                    MainAxisRegs.dir_comp.b = 1;
                if(PostionRegs.real_pos.pos_c < (ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[5] + 10000 + RunstatusRegs.ret_offset))
                    MainAxisRegs.dir_comp.c = -1;
                else
                    MainAxisRegs.dir_comp.c = 1;

                //RunstatusRegs.CBAFlag=CRunstatusRegs.BAFlag&0xFE;
                RunstatusRegs.RpdFlag = 0;
                RunstatusRegs.ReturnFlag.bit.q = 1;

                MainAxisRegs.low_speed.x = 0;
                MainAxisRegs.low_speed.y = 0;
                MainAxisRegs.low_speed.z = 0;
                MainAxisRegs.low_speed.a = 0;
                MainAxisRegs.low_speed.b = 0;
                MainAxisRegs.low_speed.c = 0;

                PostionRegs.offset_temp.pos_x = 0;
                PostionRegs.offset_temp.pos_y = 0;
                PostionRegs.offset_temp.pos_z = 0;
                PostionRegs.offset_temp.pos_a = 0;
                PostionRegs.offset_temp.pos_b = 0;
                PostionRegs.offset_temp.pos_c = 0;

                MainAxisRegs.slow_bak.x = 0;
                MainAxisRegs.slow_bak.y = 0;
                MainAxisRegs.slow_bak.z = 0;
                MainAxisRegs.slow_bak.a = 0;
                MainAxisRegs.slow_bak.b = 0;
                MainAxisRegs.slow_bak.c = 0;
            }
        }
        else
        {
            RunstatusRegs.RunStatusFlag.bit.CBAFlag=0;
            RunstatusRegs.RpdFlag = 0;
        }
    }  // glh
}
/************************************************************
//加减速程序
*************************************************************/
// S_obj:本次插补计算出的脉冲数;
// S_last:上次发送出的脉冲数
// Ei:累计误差计数器
int AccDecRoute(int32_t S_obj, int32_t *S_last, int32_t *Ei, int KL, int T, int GM)
{
    int32_t fa;           // 中间变量
    if(!S_obj)          // S_obj=0,减速
    {
        if(!*Ei)            // Ei=0,误差为零,减速完成
        {
            *S_last = 0;    // 上次发送脉冲0
            fa = 0;     // 本次发送脉冲0
//          Acc = 0;    // 本次发送脉冲0
        }
        else
        {
            fa = *Ei*2;     // fa = (Ei*2)/GM
            if((fa>0) && (fa<GM))   fa = 1;
            else if((fa<0) && (fa>-GM))     fa = -1;
            else    fa = fa/GM;

            if(labs(fa)>labs(*S_last))  // 发送脉冲大于上次发送脉冲数,本次=上次
            {
                fa = *S_last;
                *Ei = *Ei-fa;           // 累计误差-发送脉冲数
            }
            else
            {
                if((*S_last-fa)/2 > KL)     // 超过最大阶跃界限
                {
                    GM = 2*(*Ei)/(*S_last);
                    if(!GM)     fa = *Ei;   // GM=0,最后一次发送
                    else
                    {
                        fa = *Ei*2;         // FA=2*Ei/T
                        if((fa>0) && (fa<GM))       fa = 1;
                        else if((fa<0) && (fa>-GM))     fa = -1;
                        else    fa = fa/GM;
                    }
                    if(labs(fa)>labs(*S_last))      fa = *S_last;
                }
                if(labs(fa)<=labs(*S_last))
                {
                    if(GM)      GM--;
                    if(!GM)     fa = *Ei;   // 最后一次减速
                }
                *S_last = fa;
                *Ei = *Ei - fa;
            }
        }
    }
    else        // 加速
    {
        *Ei = *Ei+S_obj;
        fa = *Ei+*S_last;
        if((fa>0) && (fa<T))
            fa = 1;
        else if((fa<0) && (fa>-T))
            fa = -1;
        else
            fa = fa/T;
        *S_last = fa;
        *Ei = *Ei - fa;
    }
    return GM;
}
//RPDAccDecRegs.spd_up_par     ; // 指数加减速？
//RPDAccDecRegs.KL_e
/************************************************************
//加减速主程序
*************************************************************/
// S_obj_x:
int AccDecMain(int32_t S_obj_x, int32_t *S_last_x, int32_t *Ei_x, int GM_x, int *JJFlag)
{
    if(!*JJFlag)            // 加减速初始化
    {
        *Ei_x = 0;
        *S_last_x = 0;          // 上次发送脉冲数
        GM_x = 2*RPDAccDecRegs.spd_up_par+1;
        *JJFlag = 1;
        if(labs(S_obj_x)<=RPDAccDecRegs.KL_e)
        {
            *S_last_x = S_obj_x;
            *JJFlag = 0;
            return GM_x;
        }
    }
    GM_x = AccDecRoute(S_obj_x, S_last_x, Ei_x, RPDAccDecRegs.KL_e, RPDAccDecRegs.spd_up_par, GM_x);// 加减速过程
    if((!*Ei_x) && (!*S_last_x))
        *JJFlag = 0;
    return GM_x;
}




