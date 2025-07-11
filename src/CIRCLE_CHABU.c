#include "variabledef.h"
#include <math.h>
// 圆弧插补
// ---------------------------------------------------------------------------
struct PostionST PostionRegs;
struct InterCirST CirCleInterRegs;
int32_t real_x_bak,real_y_bak;            // 实时位置上周期备份
// ---------------------------------------------------------------------------
// 函数功能：圆弧插补预计算
// 输入参数：inter_in_cir
// 输出参数：inter_in_cir
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_in_cir cir_inter_pre(struct inter_in_cir temp)
{
    int32_t lx,ly;
    struct INT64 temp64;
// 半径计算
    if(temp.cir_r==0)               // X Y I J
    {
        if(EncodeRegs.AsReg.DaoFlag==1)                 //09.01.07
        {
            lx=temp.x_st-temp.cen_x;
            ly=temp.y_st-temp.cen_y;
            temp64 = add64(mul64(lx,lx),mul64(ly,ly));
            if(temp64.int_h == 0)
                temp.cir_r=sqr32(temp64.int_l);
            else
                temp.cir_r=sqr64(temp64);
        }
        else
            temp.cir_r=EncodeRegs.AsReg.R_cir;
    }
 // 变量初始化
    CirCleInterRegs.remx=0;
    CirCleInterRegs.remy=0;
    CirCleInterRegs.remx1=0;
    CirCleInterRegs.remy1=0;
    CirCleInterRegs.half_length.int_h=0;
    CirCleInterRegs.half_length.int_l=0;
    CirCleInterRegs.cir_half=0;
    CirCleInterRegs.cir_flag=0;
    return temp;
}
// ---------------------------------------------------------------------------
// 函数功能：圆弧插补逆圆第1组公式计算
// 输入参数：inter_pos, inter_in_cir
// 输出参数：inter_pos
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_pos cir_inter_ob1(struct inter_pos real_pos,struct inter_in_cir temp)
{
    int32_t dy01,y11,dx01,x11;
    dy01=(temp.velo*real_pos.pos_x+CirCleInterRegs.remy)/temp.cir_r;
    CirCleInterRegs.remy=(temp.velo*real_pos.pos_x+CirCleInterRegs.remy)%temp.cir_r;
    y11=real_pos.pos_y+dy01;
    dx01=(-1)*(temp.velo*y11+CirCleInterRegs.remx)/temp.cir_r;
    CirCleInterRegs.remx=(temp.velo*y11+CirCleInterRegs.remx)%temp.cir_r;
    x11=real_pos.pos_x+dx01;
    real_pos.pos_x=x11;
    real_pos.pos_y=y11;
    return real_pos;
}
// ---------------------------------------------------------------------------
// 函数功能：圆弧插补顺圆第1组公式计算
// 输入参数：inter_pos, inter_in_cir
// 输出参数：inter_pos
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_pos cir_inter_fw1(struct inter_pos real_pos,struct inter_in_cir temp)// 第1组公式计算
{
    int32_t dy01,y11,dx01,x11;
    dy01=(temp.velo*real_pos.pos_x+CirCleInterRegs.remy)/temp.cir_r;
    CirCleInterRegs.remy=(temp.velo*real_pos.pos_x+CirCleInterRegs.remy)%temp.cir_r;
    y11=real_pos.pos_y-dy01;
    dx01=(-1)*(temp.velo*y11+CirCleInterRegs.remx)/temp.cir_r;
    CirCleInterRegs.remx=(temp.velo*y11+CirCleInterRegs.remx)%temp.cir_r;
    x11=real_pos.pos_x-dx01;
    real_pos.pos_x=x11;
    real_pos.pos_y=y11;
    return real_pos;
}
// ---------------------------------------------------------------------------
// 函数功能：圆弧插补逆圆第2组公式计算
// 输入参数：inter_pos, inter_in_cir
// 输出参数：inter_pos
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_pos cir_inter_ob2(struct inter_pos real_pos,struct inter_in_cir temp)
{
    int32_t dx02,y12,dy02,x12;
    dx02=(-1)*(temp.velo*real_pos.pos_y+CirCleInterRegs.remx1)/temp.cir_r;
    CirCleInterRegs.remx1=(temp.velo*real_pos.pos_y+CirCleInterRegs.remx1)%temp.cir_r;
    x12=real_pos.pos_x+dx02;
    dy02=(temp.velo*x12+CirCleInterRegs.remy1)/temp.cir_r;
    CirCleInterRegs.remy1=(temp.velo*x12+CirCleInterRegs.remy1)%temp.cir_r;
    y12=real_pos.pos_y+dy02;
    real_pos.pos_x=x12;
    real_pos.pos_y=y12;
    return real_pos;
}
// ---------------------------------------------------------------------------
// 函数功能：圆弧插补顺圆第2组公式计算
// 输入参数：inter_pos, inter_in_cir
// 输出参数：inter_pos
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_pos cir_inter_fw2(struct inter_pos real_pos,struct inter_in_cir temp)
{
    int32_t dx02,y12,dy02,x12;
    dx02=(-1)*(temp.velo*real_pos.pos_y+CirCleInterRegs.remx1)/temp.cir_r;
    CirCleInterRegs.remx1=(temp.velo*real_pos.pos_y+CirCleInterRegs.remx1)%temp.cir_r;
    x12=real_pos.pos_x-dx02;
    dy02=(temp.velo*x12+CirCleInterRegs.remy1)/temp.cir_r;
    CirCleInterRegs.remy1=(temp.velo*x12+CirCleInterRegs.remy1)%temp.cir_r;
    y12=real_pos.pos_y-dy02;
    real_pos.pos_x=x12;
    real_pos.pos_y=y12;
    return real_pos;
}
// ---------------------------------------------------------------------------
// 函数功能：设置过半圆标志
// 输入参数：inter_pos, inter_in_cir
// 输出参数：int8
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// **********************************
int8_t set_half(struct inter_pos real_pos,struct inter_in_cir temp)
{
    int32_t x,y;
    struct INT64 t_l;
    if(real_pos.pos_x>temp.x_end)
        x=real_pos.pos_x-temp.x_end;
    else
        x=temp.x_end-real_pos.pos_x;
    if(real_pos.pos_y>temp.y_end)
        y=real_pos.pos_y-temp.y_end;
    else
        y=temp.y_end-real_pos.pos_y;
    t_l=add64(mul64(x,x),mul64(y,y));
    if(CirCleInterRegs.half_length.int_h>t_l.int_h)
    {
        CirCleInterRegs.half_length=t_l;
        return 1;
    }
    else
    {
        if(CirCleInterRegs.half_length.int_h==t_l.int_h)
        {
            if(CirCleInterRegs.half_length.int_l>t_l.int_l)
            {
                CirCleInterRegs.half_length=t_l;
                return 1;
            }
            else
            {
                CirCleInterRegs.half_length=t_l;
                return 0;
            }
        }
        else
        {
            CirCleInterRegs.half_length=t_l;
            return 0;
        }
    }
}
// ---------------------------------------------------------------------------
// 函数功能：设置圆弧插补结束标志
// 输入参数：inter_pos, inter_in_cir
// 输出参数：int8
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
int8_t set_end(struct inter_in_cir temp,struct inter_pos real_pos)
{
    struct INT64 t_l;
    if(real_pos.pos_x>temp.x_end)
        t_l.int_h=real_pos.pos_x-temp.x_end;
    else
        t_l.int_h=temp.x_end-real_pos.pos_x;
    if(real_pos.pos_y>temp.y_end)
        t_l.int_l=real_pos.pos_y-temp.y_end;
    else
        t_l.int_l=temp.y_end-real_pos.pos_y;
    if((( t_l.int_h < temp.velo ) && ( t_l.int_l < temp.velo )) || SpeedRegs.rem_length <= 0)
        return 1;
    else
        return 0;
}
// ---------------------------------------------------------------------------
// 函数功能：半径编程
// 输入参数：inter_in_cir
// 输出参数：inter_in_cir
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_in_cir cir_r_cal(struct inter_in_cir temp,int8_t G23)
{
    struct INT64 xcp,ch,r2;
    struct INT64 temp64;
    int32_t lx,ly,xc,rs1x,rs1y,bc,xadd,yadd;
    lx=labs(temp.x_st-temp.x_end);
    ly=labs(temp.y_st-temp.y_end);
    xcp=add64(mul64(lx,lx),mul64(ly,ly));
    if(xcp.int_h == 0)
        xc=sqr32(xcp.int_l);
    else
        xc=sqr64(xcp);
    ch=mul64((xc/2),(xc/2));
    r2=mul64(temp.cir_r,temp.cir_r);
    // shibo ceshi 090727
    temp64 = sub64(r2,ch);
    if(temp64.int_h == 0)
        bc=sqr32(temp64.int_l);
    else
        bc=sqr64(temp64);
    xadd = div64(mul64(temp.y_st-temp.y_end,bc),xc).int_l;
    yadd = div64(mul64(temp.x_st-temp.x_end,bc),xc).int_l;
    if(G23==3)
    {
        if(CirCleInterRegs.zuo_you==0)
        {
            rs1x = (temp.x_st+temp.x_end)/2 + xadd;
            rs1y = (temp.y_st+temp.y_end)/2 - yadd;
        }
        else
        {
            rs1x = (temp.x_st+temp.x_end)/2 - xadd;
            rs1y = (temp.y_st+temp.y_end)/2 + yadd;
        }
    }
    if(G23==2)
    {
        if(CirCleInterRegs.zuo_you==0)
        {
            rs1x = (temp.x_st+temp.x_end)/2 - xadd;
            rs1y = (temp.y_st+temp.y_end)/2 + yadd;
        }
        else
        {
            rs1x = (temp.x_st+temp.x_end)/2 + xadd;
            rs1y = (temp.y_st+temp.y_end)/2 - yadd;
        }
    }
    temp.cen_x=rs1x;
    temp.cen_y=rs1y;
    return temp;
}

// ---------------------------------------------------------------------------
// 函数功能：顺圆插补
// 输入参数：无
// 输出参数：无
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------

void Shun_Cir(void)
{
//  struct Robot_T6 T6_test1;
//  struct inter_pos PostionRegs.real_pos_test;//chenlin2014.3.11
    real_x_bak=PostionRegs.real_pos2.pos_x;
    real_y_bak=PostionRegs.real_pos2.pos_y;
    if(RunstatusRegs.run_cnt==1)
    {
        if(EncodeRegs.AsReg.Plate==17)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_x;   // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.AsReg.X;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.AsReg.Y;
            CirCleInterRegs.st_end_cir.cen_x=EncodeRegs.AsReg.I+PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.cen_y=EncodeRegs.AsReg.J+PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.velo=EncodeRegs.AsReg.F;
        }
        if(EncodeRegs.AsReg.Plate==18)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_z;   // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.AsReg.Z;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.AsReg.X;
            CirCleInterRegs.st_end_cir.cen_x=EncodeRegs.AsReg.K+PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.cen_y=EncodeRegs.AsReg.I+PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.velo=EncodeRegs.AsReg.F;
        }
        if(EncodeRegs.AsReg.Plate==19)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_y;   // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.AsReg.Y;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.AsReg.Z;
            CirCleInterRegs.st_end_cir.cen_x=EncodeRegs.AsReg.J+PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.cen_y=EncodeRegs.AsReg.K+PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.velo=EncodeRegs.AsReg.F;
        }
        CirCleInterRegs.st_end_cir=cir_inter_pre(CirCleInterRegs.st_end_cir);
        PostionRegs.real_pos3.pos_x=CirCleInterRegs.st_end_cir.x_st;
        PostionRegs.real_pos3.pos_y=CirCleInterRegs.st_end_cir.y_st;
        PostionRegs.real_pos1.pos_x=CirCleInterRegs.st_end_cir.x_st;
        PostionRegs.real_pos1.pos_y=CirCleInterRegs.st_end_cir.y_st;
        speed_pre();
        RunstatusRegs.run_cnt=0;
    }
    speed_control();
    CirCleInterRegs.st_end_cir.velo=SpeedRegs.sect_in_feed_BK;
    if(CirCleInterRegs.cir_flag!=1)
    {
        PostionRegs.real_pos3.pos_x=PostionRegs.real_pos3.pos_x-CirCleInterRegs.st_end_cir.cen_x;
        PostionRegs.real_pos3.pos_y=PostionRegs.real_pos3.pos_y-CirCleInterRegs.st_end_cir.cen_y;
        PostionRegs.real_pos3=cir_inter_fw1(PostionRegs.real_pos3,CirCleInterRegs.st_end_cir);
        PostionRegs.real_pos3.pos_x=PostionRegs.real_pos3.pos_x+CirCleInterRegs.st_end_cir.cen_x;
        PostionRegs.real_pos3.pos_y=PostionRegs.real_pos3.pos_y+CirCleInterRegs.st_end_cir.cen_y;
        PostionRegs.real_pos1.pos_x=PostionRegs.real_pos1.pos_x-CirCleInterRegs.st_end_cir.cen_x;
        PostionRegs.real_pos1.pos_y=PostionRegs.real_pos1.pos_y-CirCleInterRegs.st_end_cir.cen_y;
        PostionRegs.real_pos1=cir_inter_fw2(PostionRegs.real_pos1,CirCleInterRegs.st_end_cir);
        PostionRegs.real_pos1.pos_x=PostionRegs.real_pos1.pos_x+CirCleInterRegs.st_end_cir.cen_x;
        PostionRegs.real_pos1.pos_y=PostionRegs.real_pos1.pos_y+CirCleInterRegs.st_end_cir.cen_y;
        PostionRegs.real_pos2.pos_x=(PostionRegs.real_pos3.pos_x+PostionRegs.real_pos1.pos_x)/2;
        PostionRegs.real_pos2.pos_y=(PostionRegs.real_pos3.pos_y+PostionRegs.real_pos1.pos_y)/2;
        if(CirCleInterRegs.cir_first==1)
        {   //090728
            if((labs(PostionRegs.real_pos2.pos_x-CirCleInterRegs.st_end_cir.cen_x))>CirCleInterRegs.st_end_cir.cir_r)
                PostionRegs.real_pos2.pos_x=real_x_bak;
            if((labs(PostionRegs.real_pos2.pos_y-CirCleInterRegs.st_end_cir.cen_y))>CirCleInterRegs.st_end_cir.cir_r)
                PostionRegs.real_pos2.pos_y=real_y_bak;
        }
        CirCleInterRegs.cir_first=1;
    }
    else
    {
        PostionRegs.real_pos2.pos_x=CirCleInterRegs.st_end_cir.x_end;
        PostionRegs.real_pos2.pos_y=CirCleInterRegs.st_end_cir.y_end;
    }
    if(CirCleInterRegs.cir_flag!=1)
    {
        if(CirCleInterRegs.cir_half==1)
            CirCleInterRegs.cir_flag=set_end(CirCleInterRegs.st_end_cir,PostionRegs.real_pos2);
        CirCleInterRegs.cir_half=set_half(PostionRegs.real_pos2, CirCleInterRegs.st_end_cir);
    } // 圆弧测试
    if(CirCleInterRegs.cir_flag==1)
    {
        PostionRegs.real_pos2.pos_x=CirCleInterRegs.st_end_cir.x_end;
        PostionRegs.real_pos2.pos_y=CirCleInterRegs.st_end_cir.y_end;
        RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
        CirCleInterRegs.st_end_cir.cir_r=0;
        CirCleInterRegs.st_end_cir.velo=0;
    }
   if(EncodeRegs.AsReg.Plate==17)
   {
       PostionRegs.real_pos.pos_x=PostionRegs.real_pos2.pos_x;
       PostionRegs.real_pos.pos_y=PostionRegs.real_pos2.pos_y;
   }
   if(EncodeRegs.AsReg.Plate==18)
   {
       PostionRegs.real_pos.pos_z=PostionRegs.real_pos2.pos_x;
       PostionRegs.real_pos.pos_x=PostionRegs.real_pos2.pos_y;
   }
   if(EncodeRegs.AsReg.Plate==19)
   {
       PostionRegs.real_pos.pos_y=PostionRegs.real_pos2.pos_x;
       PostionRegs.real_pos.pos_z=PostionRegs.real_pos2.pos_y;
   }
   if(EncodeRegs.AsReg.Plate==17 && (PostionRegs.real_pos.pos_x == EncodeRegs.AsReg.X) && (PostionRegs.real_pos.pos_y == EncodeRegs.AsReg.Y) )
   {
       RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
       CirCleInterRegs.cir_first=0;
   }
   if(EncodeRegs.AsReg.Plate==18 && (PostionRegs.real_pos.pos_z == EncodeRegs.AsReg.Z) && (PostionRegs.real_pos.pos_x == EncodeRegs.AsReg.X) )
   {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
        CirCleInterRegs.cir_first=0;
   }
   if(EncodeRegs.AsReg.Plate==19 && (PostionRegs.real_pos.pos_y == EncodeRegs.AsReg.Y) && (PostionRegs.real_pos.pos_z == EncodeRegs.AsReg.Z) )
   {
       RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
       CirCleInterRegs.cir_first=0;
   } //glh
}

// ---------------------------------------------------------------------------
// 函数功能：逆膊
// 输入参数：无
// 输出参数：无
// 创建时间：2006.03.27
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------

void Ni_Cir(void)
{
    real_x_bak=PostionRegs.real_pos2.pos_x;
    real_y_bak=PostionRegs.real_pos2.pos_y;
    if(RunstatusRegs.run_cnt==1)
    {
        if(EncodeRegs.AsReg.Plate==17)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_x;     // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_y;     // 起点
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.AsReg.X;            // 终点
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.AsReg.Y;
            CirCleInterRegs.st_end_cir.cen_x=EncodeRegs.AsReg.I+PostionRegs.real_pos.pos_x; // 圆心坐标
            CirCleInterRegs.st_end_cir.cen_y=EncodeRegs.AsReg.J+PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.velo=EncodeRegs.AsReg.F;             // 速度
        }
        if(EncodeRegs.AsReg.Plate==18)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_z;     // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.AsReg.Z;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.AsReg.X;
            CirCleInterRegs.st_end_cir.cen_x=EncodeRegs.AsReg.K+PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.cen_y=EncodeRegs.AsReg.I+PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.velo=EncodeRegs.AsReg.F;
        }
        if(EncodeRegs.AsReg.Plate==19)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_y;   // 平面在此处加判断
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.AsReg.Y;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.AsReg.Z;
            CirCleInterRegs.st_end_cir.cen_x=EncodeRegs.AsReg.J+PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.cen_y=EncodeRegs.AsReg.K+PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.velo=EncodeRegs.AsReg.F;
        }
        CirCleInterRegs.st_end_cir=cir_inter_pre(CirCleInterRegs.st_end_cir);
        PostionRegs.real_pos3.pos_x=CirCleInterRegs.st_end_cir.x_st;
        PostionRegs.real_pos3.pos_y=CirCleInterRegs.st_end_cir.y_st;
        PostionRegs.real_pos1.pos_x=CirCleInterRegs.st_end_cir.x_st;
        PostionRegs.real_pos1.pos_y=CirCleInterRegs.st_end_cir.y_st;
        speed_pre();
        RunstatusRegs.run_cnt=0;
    }
    speed_control();
    CirCleInterRegs.st_end_cir.velo=SpeedRegs.sect_in_feed_BK;
    if(CirCleInterRegs.cir_flag!=1)
    {
//        PostionRegs.real_pos3.pos_x=PostionRegs.real_pos.pos_x-CirCleInterRegs.st_end_cir.cen_x;            //第一步
//        PostionRegs.real_pos.pos_y=PostionRegs.real_pos.pos_y-CirCleInterRegs.st_end_cir.cen_y;
//        PostionRegs.real_pos3=cir_inter_ob1(PostionRegs.real_pos3,CirCleInterRegs.st_end_cir);
//        PostionRegs.real_pos.pos_x=PostionRegs.real_pos.pos_x+CirCleInterRegs.st_end_cir.cen_x;
//        PostionRegs.real_pos.pos_y=PostionRegs.real_pos.pos_y+CirCleInterRegs.st_end_cir.cen_y;

        PostionRegs.real_pos3.pos_x=PostionRegs.real_pos3.pos_x-CirCleInterRegs.st_end_cir.cen_x;            //第一步
        PostionRegs.real_pos3.pos_y=PostionRegs.real_pos3.pos_y-CirCleInterRegs.st_end_cir.cen_y;
        PostionRegs.real_pos3=cir_inter_ob1(PostionRegs.real_pos3,CirCleInterRegs.st_end_cir);
        PostionRegs.real_pos3.pos_x=PostionRegs.real_pos3.pos_x+CirCleInterRegs.st_end_cir.cen_x;
        PostionRegs.real_pos3.pos_y=PostionRegs.real_pos3.pos_y+CirCleInterRegs.st_end_cir.cen_y;

        PostionRegs.real_pos1.pos_x=PostionRegs.real_pos1.pos_x-CirCleInterRegs.st_end_cir.cen_x;           //第二步
        PostionRegs.real_pos1.pos_y=PostionRegs.real_pos1.pos_y-CirCleInterRegs.st_end_cir.cen_y;
        PostionRegs.real_pos1=cir_inter_ob2(PostionRegs.real_pos1,CirCleInterRegs.st_end_cir);
        PostionRegs.real_pos1.pos_x=PostionRegs.real_pos1.pos_x+CirCleInterRegs.st_end_cir.cen_x;
        PostionRegs.real_pos1.pos_y=PostionRegs.real_pos1.pos_y+CirCleInterRegs.st_end_cir.cen_y;

        PostionRegs.real_pos2.pos_x=(PostionRegs.real_pos.pos_x+PostionRegs.real_pos1.pos_x)/2;
        PostionRegs.real_pos2.pos_y=(PostionRegs.real_pos3.pos_y+PostionRegs.real_pos1.pos_y)/2;
        if(CirCleInterRegs.cir_first==1)
        {   //090728
            if((labs(PostionRegs.real_pos2.pos_x-CirCleInterRegs.st_end_cir.cen_x))>CirCleInterRegs.st_end_cir.cir_r)
                PostionRegs.real_pos2.pos_x=real_x_bak;
            if((labs(PostionRegs.real_pos2.pos_y-CirCleInterRegs.st_end_cir.cen_y))>CirCleInterRegs.st_end_cir.cir_r)
                PostionRegs.real_pos2.pos_y=real_y_bak;
        }
        CirCleInterRegs.cir_first=1;
    }
    else
    {
        PostionRegs.real_pos2.pos_x=CirCleInterRegs.st_end_cir.x_end;
        PostionRegs.real_pos2.pos_y=CirCleInterRegs.st_end_cir.y_end;
    }
    if(CirCleInterRegs.cir_flag!=1)
    {
        if(CirCleInterRegs.cir_half==1)
            CirCleInterRegs.cir_flag=set_end(CirCleInterRegs.st_end_cir,PostionRegs.real_pos2);
        CirCleInterRegs.cir_half=set_half(PostionRegs.real_pos2, CirCleInterRegs.st_end_cir);
    } // 圆弧测试
    if(CirCleInterRegs.cir_flag==1)
    {
        PostionRegs.real_pos2.pos_x=CirCleInterRegs.st_end_cir.x_end;
        PostionRegs.real_pos2.pos_y=CirCleInterRegs.st_end_cir.y_end;
        RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
        CirCleInterRegs.st_end_cir.cir_r=0;
        CirCleInterRegs.st_end_cir.velo=0;
    }
    if(EncodeRegs.AsReg.Plate==17)
    {
        PostionRegs.real_pos.pos_x=PostionRegs.real_pos2.pos_x;
        PostionRegs.real_pos.pos_y=PostionRegs.real_pos2.pos_y;
    }
    if(EncodeRegs.AsReg.Plate==18)
    {
        PostionRegs.real_pos.pos_z=PostionRegs.real_pos2.pos_x;
        PostionRegs.real_pos.pos_x=PostionRegs.real_pos2.pos_y;
    }
    if(EncodeRegs.AsReg.Plate==19)
    {
        PostionRegs.real_pos.pos_y=PostionRegs.real_pos2.pos_x;
        PostionRegs.real_pos.pos_z=PostionRegs.real_pos2.pos_y;
    }
    if(EncodeRegs.AsReg.Plate==17 && PostionRegs.real_pos.pos_x == EncodeRegs.AsReg.X && PostionRegs.real_pos.pos_y == EncodeRegs.AsReg.Y)
    {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
        CirCleInterRegs.cir_first=0;
    }
    if(EncodeRegs.AsReg.Plate==18 && PostionRegs.real_pos.pos_z == EncodeRegs.AsReg.Z && PostionRegs.real_pos.pos_x == EncodeRegs.AsReg.X)
    {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
        CirCleInterRegs.cir_first=0;
    }
    if(EncodeRegs.AsReg.Plate==19 && PostionRegs.real_pos.pos_y == EncodeRegs.AsReg.Y && PostionRegs.real_pos.pos_z == EncodeRegs.AsReg.Z)
    {
        RunstatusRegs.RunStatusFlag.bit.CBAFlag= 0;
        CirCleInterRegs.cir_first=0;
    }
}

// ---------------------------------------------------------------------------
// 函数功能：   半径处理
// 输入参数：   无
// 输出参数：   无
// 创建时间：   2006.09.22
// 最后修改时间:2006.09.22
// 修改原因：
// 创建人  ：   刘世杰
// ---------------------------------------------------------------------------
void RadiusDeal()       //把R换为 I K J;
{
    if(EncodeRegs.BsReg.R>0)
        CirCleInterRegs.zuo_you = 0;              //劣弧
    else
    {
        CirCleInterRegs.zuo_you = 1;              //优弧
        EncodeRegs.BsReg.R = labs(EncodeRegs.BsReg.R);
    }

    if(EncodeRegs.BsReg.R != 0)
    {
        if(EncodeRegs.BsReg.GX7 == 17)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.BsReg.X;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.BsReg.Y;
        }
        if(EncodeRegs.BsReg.GX7 == 18)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_x;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.BsReg.Z;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.BsReg.X;
        }
        if(EncodeRegs.BsReg.GX7 == 19)
        {
            CirCleInterRegs.st_end_cir.x_st=PostionRegs.real_pos.pos_y;
            CirCleInterRegs.st_end_cir.y_st=PostionRegs.real_pos.pos_z;
            CirCleInterRegs.st_end_cir.x_end=EncodeRegs.BsReg.Y;
            CirCleInterRegs.st_end_cir.y_end=EncodeRegs.BsReg.Z;
        }

        CirCleInterRegs.st_end_cir.cir_r=EncodeRegs.BsReg.R;
        CirCleInterRegs.st_end_cir=cir_r_cal(CirCleInterRegs.st_end_cir,EncodeRegs.BsReg.GX2);
/*      if(BsReg.GX7 == 17)
        {
            BsReg.I=st_end_cir.cen_x-st_end_cir.x_st;
            BsReg.J=st_end_cir.cen_y-st_end_cir.y_st;
        }
        if(BsReg.GX7 == 18)
        {
            BsReg.K=st_end_cir.cen_x-st_end_cir.x_st;
            BsReg.I=st_end_cir.cen_y-st_end_cir.y_st;
        }
        if(BsReg.GX7 == 19)
        {
            BsReg.J=st_end_cir.cen_x-st_end_cir.x_st;
            BsReg.K=st_end_cir.cen_y-st_end_cir.y_st;
        }
        BsReg.R = 0;*/
    }
}

// ---------------------------------------------------------------------------
// 函数功能：   半径处理
// 输入参数：   无
// 输出参数：   无
// 创建时间：   2006.09.22
// 最后修改时间:2006.09.22
// 修改原因：
// 创建人  ：   刘世杰
// ---------------------------------------------------------------------------
struct SpeedSTRRegs CsReg_RadiusDeal(struct SpeedSTRRegs sect_Temp)     //把R换为 I K J;
{
    struct inter_in_cir st_end_cir_Temp;
    struct INT64 temp64;
    int32_t lx,ly;
    if(sect_Temp.R>0)
        CirCleInterRegs.zuo_you = 0;              //劣弧
    else
    {
        CirCleInterRegs.zuo_you = 1;              //优弧
        sect_Temp.R = labs(sect_Temp.R);
    }

    if(sect_Temp.R != 0)
    {
        if(sect_Temp.plane == 17)
        {
            st_end_cir_Temp.x_st=EncodeRegs.BsReg.X;
            st_end_cir_Temp.y_st=EncodeRegs.BsReg.Y;
            st_end_cir_Temp.x_end=sect_Temp.end.pos_x;
            st_end_cir_Temp.y_end=sect_Temp.end.pos_y;

        }
        if(sect_Temp.plane == 18)
        {
            st_end_cir_Temp.x_st=EncodeRegs.BsReg.Z;
            st_end_cir_Temp.y_st=EncodeRegs.BsReg.X;
            st_end_cir_Temp.x_end=sect_Temp.end.pos_z;
            st_end_cir_Temp.y_end=sect_Temp.end.pos_x;
        }
        if(sect_Temp.plane == 19)
        {
            st_end_cir_Temp.x_st=EncodeRegs.BsReg.Y;
            st_end_cir_Temp.y_st=EncodeRegs.BsReg.Z;
            st_end_cir_Temp.x_end=sect_Temp.end.pos_y;
            st_end_cir_Temp.y_end=sect_Temp.end.pos_z;
        }

        st_end_cir_Temp.cir_r=sect_Temp.R;
        st_end_cir_Temp=cir_r_cal(st_end_cir_Temp,sect_Temp.type);
        if(sect_Temp.plane == 17)
        {
            sect_Temp.i=st_end_cir_Temp.cen_x-st_end_cir_Temp.x_st;
            sect_Temp.j=st_end_cir_Temp.cen_y-st_end_cir_Temp.y_st;
        }
        if(sect_Temp.plane == 18)
        {
            sect_Temp.k=st_end_cir_Temp.cen_x-st_end_cir_Temp.x_st;
            sect_Temp.i=st_end_cir_Temp.cen_y-st_end_cir_Temp.y_st;
        }
        if(sect_Temp.plane == 19)
        {
            sect_Temp.j=st_end_cir_Temp.cen_x-st_end_cir_Temp.x_st;
            sect_Temp.k=st_end_cir_Temp.cen_y-st_end_cir_Temp.y_st;
        }
        sect_Temp.R = 0;
    }

    //////////////////////////////// 09.1.3
    EncodeRegs.CsReg.R_cir=0;
    if ((sect_Temp.type ==2)||(sect_Temp.type ==3))
    {
        //st_end_cir_Temp.cir_r=sqr64(add64(mul64(lx,lx),mul64(ly,ly)));
        if(sect_Temp.plane == 17)
        {
            lx=sect_Temp.i;
            ly=sect_Temp.j;
        }
        if(sect_Temp.plane == 18)
        {
            lx=sect_Temp.k;
            ly=sect_Temp.i;
        }
        if(sect_Temp.plane == 19)
        {
            lx=sect_Temp.j;
            ly=sect_Temp.k;
        }
        // shibo ceshi 090727
        temp64.int_h = 0;
        temp64.int_l = 0;
        temp64 = add64(mul64(lx,lx),mul64(ly,ly));
        if(temp64.int_h == 0)
            st_end_cir_Temp.cir_r=sqr32(temp64.int_l);
        else
            st_end_cir_Temp.cir_r=sqr64(temp64);
        EncodeRegs.CsReg.R_cir=st_end_cir_Temp.cir_r;     // 09.1.3 计惆刖�
        EncodeRegs.CsReg.R = EncodeRegs.CsReg.R_cir;//chenlin2014.4.25 chenlin2014.5.29
    }
    //////////////////////////////
    return sect_Temp;
}

// ===========================================================================
// No more.
// ===========================================================================
