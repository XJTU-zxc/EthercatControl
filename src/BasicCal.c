#include "variabledef.h"
#include <stdlib.h>
// 基本运算操作
//---------------------------------------------------------------------------
// 64位减法程序
struct INT64 sub64(struct INT64 temp1,struct INT64 temp2)
{
    int32_t flag=0;
    int32_t ram1,ram2;
    struct INT64 ram3;
    if(temp1.int_l>=temp2.int_l)
        ram1=temp1.int_l-temp2.int_l;
    else
    {
        ram1=0xffffffff-temp2.int_l+temp1.int_l+1;
        flag=1;
    }
    ram2=temp1.int_h-temp2.int_h-flag;
    ram3.int_l=ram1;
    ram3.int_h=ram2;
    return ram3;
}
// 64位乘法程序
struct INT64 mul64(int32_t x1,int32_t x2)
{
    struct INT64 temp1;
    struct INT64 temp0;
    int32_t tp1h,tp2h;
    uint32_t tp1l,tp2l;
    int32_t x1y1,x2y1,x1y2,x2y2,temp=0;
    temp0.int_l = 0;
    temp0.int_h = 0;
    if(x1==0||x2==0)
    {
        temp1.int_l=0;
        temp1.int_h=0;
        return temp1;
    }
    temp = labs(x1);
    tp1h=(temp>>16)&0xffff;
    tp1l=temp&0xffff;
    temp = labs(x2);
    tp2h=(temp>>16)&0xffff;
    tp2l=temp&0xffff;
    x1y1=tp1l*tp2l;
    temp=(x1y1>>16)&0xffff;
    x2y1=(tp2l*tp1h)+temp;
    x1y2=tp2h*tp1l+x2y1;
    temp=(x1y2>>16)&0xffff;
    x2y2=tp2h*tp1h+temp;
    temp1.int_l=(x1y1&0xffff)|(x1y2<<16);
    temp1.int_h=x2y2;         //+((tp1l&tp2l&0x8000)>>15);
    if((x1 >0 && x2 < 0) || (x1 < 0 && x2 > 0))
        temp1 = sub64(temp0, temp1);
    return temp1;
}
// 64位加法程序
struct INT64 add64(struct INT64 temp1,struct INT64 temp2)
{
    int32_t flag=0;
    int32_t ram1,ram2;
    struct INT64 ram3;
    ram1=((temp1.int_l&0xf0000000)>>28)+((temp2.int_l&0xf0000000)>>28);
    ram2=(((temp1.int_l&0xfffffff)+(temp2.int_l&0xfffffff))&0xf0000000)>>28;
    if(ram1+ram2>15)
        flag=1;
    ram3.int_l=temp1.int_l+temp2.int_l;
    ram3.int_h=temp1.int_h+temp2.int_h+flag;
    return ram3;
}

// 64位除法程序
struct INT64 div64(struct INT64 temp1,int32_t temp2)
{
    int k = 1;
    int16_t m=32;
    int32_t int_h;
    uint32_t int_l;
    struct INT64 temp0;
    temp0.int_l = 0;
    temp0.int_h = 0;

    if(temp2==0)
        return temp1;


    if(((temp1.int_h < 0) && (temp2 > 0)) || ((temp1.int_h >= 0) && (temp2 < 0)))
        k = -1;
    if(temp1.int_h < 0)
        temp1 = sub64(temp0, temp1);

    int_l=temp1.int_l;
    int_h=temp1.int_h;
    while(m!=0)
    {
        int_h=((int_h)<<1);
        if((int_l&0x80000000)!=0)
        int_h=(int_h|0x1);
        int_l=(int_l<<1);
        if(int_h-labs(temp2)>=0)
        {
            int_h=int_h-labs(temp2);
            int_l++;
        }
        m=m-1;
    }
    temp1.int_l=int_l;              
    temp1.int_h=int_h;              
    if(k == -1)
        temp1 = sub64(temp0, temp1);
    return temp1;
}

// 64位开方程序
int32_t sqr64(struct INT64 temp)
{
    int32_t init_value,result1=0,result2=0;
    if(temp.int_h<0)
        return 0;
    if(temp.int_l==0&&temp.int_h==0)
        return 0;
    if(temp.int_l==1&&temp.int_h==0)
        return 1;
    if(temp.int_h==0 )
        init_value=2;
    else
    {
        if(((temp.int_h&0xffff)!=0)&&((temp.int_h&0xffff0000)==0))
            init_value=0xffff;
        else
            init_value=0xffffff;
    }
    result2=init_value;
    do
    {
        result1=result2;
        result2=(div64(temp,result1).int_l+result1)>>1;
    }
    while(labs(result1-result2)>1);

    return result2;
}


// 32位开方程序     090723
int32_t sqr32(int32_t temp)
{
    uint32_t result1=0,result2=0,temp32;
    if(temp<0)
        return 0;
    if(temp==0)
        return 0;
    if(temp==1)
        return 1;
    result2=2;
    do
    {
        result1=result2;
        temp32 = temp/result1;
        result2=(temp32+result1)>>1;
    }
    while(labs(result1-result2)>1);

    return result2;
}


// 64*32位乘法程序
struct INT64 mul64X32(struct INT64 x1,int32_t x2)
{
    struct INT64 temp1,temp2,temp3,temp4;
    int32_t temp;
    temp1.int_h=0;
    temp1.int_l=0;
    if(( x2 == 0 )||( x1.int_h == 0 && x1.int_l == 0 ))
        return temp1;
    temp = labs(x2);
    if( x1.int_h < 0 )
        temp2 = sub64(temp1,x1);
    else
    {
        temp2.int_h=x1.int_h;
        temp2.int_l=x1.int_l;
    }
    temp3 = mul64(temp2.int_l,temp);        //x1.int_l*x2
    temp4 = mul64(temp2.int_h,temp);        //x1.int_h*x2
    temp3.int_h = temp3.int_h + temp4.int_l;
    if(( x1.int_h < 0 && x2 >0 )||( x1.int_h >= 0 && x2 < 0 ))
        temp3 = sub64(temp1,temp3);
    return temp3;
}


//64位除以64位程序
struct INT64 div64X64(struct INT64 x1,struct INT64 x2)
{
    struct INT64 temp1,temp2,temp3,temp4,ret;
    int32_t m=64,quotient;
    temp1.int_h = 0;
    temp1.int_l = 0;
    ret.int_h=0;
    ret.int_l=0;
    quotient = 0;
    if(( x2.int_h == 0 )&&( x2.int_l ==0 ))
        return ret;
    if( x1.int_h < 0 )
        temp2=sub64(temp1,x1);
    else
    {
        temp2.int_h = x1.int_h;
        temp2.int_l = x1.int_l;
    }
    if( x2.int_h < 0 )
        temp3=sub64(temp1,x2);
    else
    {
        temp3.int_h = x2.int_h;
        temp3.int_l = x2.int_l;
    }
    while(m!=0)
    {
        ret.int_h = ret.int_h << 1;
        if( (ret.int_l&0x80000000) != 0)
            ret.int_h = ret.int_h | 0x01;
        ret.int_l = ret.int_l << 1;
        if( (temp2.int_h&0x80000000) != 0)
            ret.int_l = ret.int_l | 0x01;

        temp2.int_h = temp2.int_h << 1;
        if( (temp2.int_l&0x80000000) != 0 )
            temp2.int_h = temp2.int_h | 0x1;
        temp2.int_l = temp2.int_l << 1;
        quotient = quotient << 1;
        temp4 = sub64(ret,temp3);
        if(temp4.int_h >= 0)
        {
            ret.int_h = temp4.int_h;
            ret.int_l = temp4.int_l;
            quotient++;
        }

        m = m-1;
    }
    ret.int_l = quotient;
    if((( x1.int_h >= 0 )&&( x2.int_h < 0 ))||(( x1.int_h < 0 )&&( x2.int_h >= 0 )))
        ret.int_l = 0 - ret.int_l;
    return ret;
}
