//刀补

#include "variabledef.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define  True  1
#define  False 0
//刀补定义结构体

extern int8_t BitFlagBK;
struct DbSTREGS DBRegs;
//********************************************
//函数功能：   计算转接点
//输入参数：   //输入参数：G＝0－－－G17，1－－G18，2－－G19；circle＝0－－－直线
//              G4x=0 左补偿 ＝1－－右补偿，d－－刀具半径；
//              Line1 直线一的数据信息，slope1，slope2两段线段的斜率信息
//输出参数:     返回转接点的值
//创建时间：   2006.03.03
//最后修改时间:2005.03.03
//修改原因：   无
//创建人  ：   刘世杰
//********************************************
struct RTNDATA JointPoint(int G,struct S_Line Line1,int32_t d,struct S_Slope slope1,struct S_Slope slope2)
{
    struct RTNDATA Join_Point;
    struct INT64 temp1,temp2,temp3;
    if( G == 0 )
    {
        temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
        temp2 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
        temp3 = div64X64(temp1,temp2);
        Join_Point.y=temp3.int_l+Line1.Y1;
        temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
        temp3 = div64X64(temp1,temp2);
        Join_Point.x=temp3.int_l+Line1.X1;
    }
    if( G == 1 )
    {
        //Join_Point.x=(slope2.Xl-slope1.Xl)*d/(slope1.Zl*slope2.Xl-slope2.Zl*slope1.Xl)+Line1.X1;
        //Join_Point.z=(slope2.Zl-slope1.Zl)*d/(slope1.Zl*slope2.Xl-slope2.Zl*slope1.Xl)+Line1.Z1;
        temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
        temp2 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
        temp3 = div64X64(temp1,temp2);
        Join_Point.x=temp3.int_l+Line1.X1;
        temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
        temp3 = div64X64(temp1,temp2);
        Join_Point.z=temp3.int_l+Line1.Z1;
    }
    if( G == 2 )
    {
        //Join_Point.z=(slope2.Zl-slope1.Zl)*d/(slope1.Yl*slope2.Zl-slope2.Yl*slope1.Zl)+Line1.Z1;
        //Join_Point.y=(slope2.Yl-slope1.Yl)*d/(slope1.Yl*slope2.Zl-slope2.Yl*slope1.Zl)+Line1.Y1;
        temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
        temp2 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
        temp3 = div64X64(temp1,temp2);
        Join_Point.z=temp3.int_l+Line1.Z1;
        temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
        temp3 = div64X64(temp1,temp2);
        Join_Point.y=temp3.int_l+Line1.Y1;
    }
    return Join_Point;
}
//********************************************
//函数功能：   对字符串进行译码
//输入参数：   char *EncodeChar
//输出参数类型:void
//创建时间：   2006.03.03
//最后修改时间:2005.03.03
//修改原因：   由于里面用的是Bs寄存器所以无返回参数
//创建人  ：   杨晓龙
//********************************************
void StrEncode(char *EncodeChar)
{
    char    *p,ch;
    uint16_t  Gcode=0;
    p = EncodeChar;
    while(*p!=0)
    {
        ch=*p;
        switch(ch)
        {
        case 'X':
            EncodeRegs.BsReg.X = atol(p+1);
            BitFlagBK = BitFlagBK | 0x01;
            break;
        case 'Y':
            EncodeRegs.BsReg.Y = atol(p+1);
            BitFlagBK = BitFlagBK | 0x02;
            break;
        case 'Z':
            EncodeRegs.BsReg.Z = atol(p+1);
            BitFlagBK = BitFlagBK | 0x04;
            break;
        case 'A':
            EncodeRegs.BsReg.A = atol(p+1);
            BitFlagBK = BitFlagBK | 0x08;
            break;
        case 'B':
            EncodeRegs.BsReg.B = atol(p+1);
            BitFlagBK = BitFlagBK | 0x10;
            break;
        case 'C':
            EncodeRegs.BsReg.C = atol(p+1);
            BitFlagBK = BitFlagBK | 0x20;
            break;
        case 'I':
            EncodeRegs.BsReg.I = atol(p+1);
            break;
        case 'J':
            EncodeRegs.BsReg.J = atol(p+1);
            break;
        case 'K':
            EncodeRegs.BsReg.K = atol(p+1);
            break;
        case 'F':
            EncodeRegs.BsReg.F = atol(p+1);
            break;
        case 'G':
            Gcode = atol(p+1);
            switch (Gcode)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                EncodeRegs.BsReg.GX2 = Gcode;
                break;
            case 17:
            case 18:
            case 19:
                EncodeRegs.BsReg.GX7 = Gcode;
                break;
            }
            break;
        }
        p++;
    }
}
//********************************************
//函数功能：   刀补代码的计算方向矢量
//输入参数：   无
//输出参数类型:S_Slope
//创建时间：   2006.02.15
//最后修改时间:2005.02.15
//修改原因：返回类型修改(注:必须加上struct)
//          这里需要用DIV64
//创建人  ：   刘世杰
//********************************************
struct S_Slope CalSlope(int G,struct S_Line Line,int circle)
{
    int32_t   I,J,K;
    int32_t   X0,X1,Y0,Y1,Z0,Z1;
    int     a;
    struct  S_Slope slope;
    a=1;
    slope.G=G;
    if(circle==0)///直线
    {
        X0=Line.X0;
        X1=Line.X1;
        Y0=Line.Y0;
        Y1=Line.Y1;
        Z0=Line.Z0;
        Z1=Line.Z1;
        if(G==0)
        {
            slope.length = sqr64(add64(mul64(X1-X0,X1-X0),mul64(Y1-Y0,Y1-Y0)));
            slope.Xl=X1-X0;
            slope.Yl=Y1-Y0;
            slope.Zl=0;
        }
        if(G==1)
        {
            slope.length = sqr64(add64(mul64(X1-X0,X1-X0),mul64(Z1-Z0,Z1-Z0)));
            slope.Xl=X1-X0;
            slope.Zl=Z1-Z0;
            slope.Yl=0;
        }
        if(G==2)
        {
            slope.length = sqr64(add64(mul64(Z1-Z0,Z1-Z0),mul64(Y1-Y0,Y1-Y0)));
            slope.Zl=Z1-Z0;
            slope.Yl=Y1-Y0;
            slope.Xl=0;
        }
    }///end circle==0
    if(circle==1)///圆弧起点斜率
    {

        if(Line.type==3) a=-1;
        I=Line.I;J=Line.J;K=Line.K;
        if(G==0) ///XY
        {
            slope.length=a*sqr64(add64(mul64(I,I),mul64(J,J)));
            slope.Yl=I;
            slope.Xl=(-1)*J;
            slope.Zl=0;
        }
        if(G==1) ///ZX
        {
            slope.length=a*sqr64(add64(mul64(I,I),mul64(K,K)));
            slope.Xl=K;
            slope.Zl=(-1)*I;
            slope.Yl=0;
        }
        if(G==2) ///YZ
        {
            slope.length=a*sqr64(add64(mul64(K,K),mul64(J,J)));
            slope.Zl=J;
            slope.Yl=(-1)*K;
            slope.Xl=0;
        }

    }///end circle==1
    if(circle==2)///圆弧终点斜率
    {
        if(Line.type==3) a=-1;
        I=Line.I;J=Line.J;K=Line.K;
        if(G==0) ///XY
        {
            slope.length=a*sqr64(add64(mul64(I,I),mul64(J,J)));
            slope.Yl=Line.X0+I-Line.X1;
            slope.Xl=(-1)*(Line.Y0+J-Line.Y1);
            slope.Zl=0;
        }
        if(G==1) ///ZX
        {
            slope.length=a*sqr64(add64(mul64(I,I),mul64(J,J)));
            slope.Xl=Line.Z0+K-Line.Z1;
            slope.Zl=(-1)*(Line.X0+I-Line.X1);
            slope.Yl=0;
        }
        if(G==2) ///YZ
        {
            slope.length=a*sqr64(add64(mul64(K,K),mul64(J,J)));
            slope.Zl=Line.Y0+J-Line.Y1;
            slope.Yl=(-1)*(Line.Z0+K-Line.Z1);
            slope.Xl=0;
        }

    }///end circle==2
    return slope;
}
//********************************************
//函数功能：   刀具补偿转接类型的判断
//输入参数：   G4x=0 左补偿 ＝1－－右补偿，
//             d－－刀具半径；
//输出参数类型:int  0－－－缩短形，1－－－伸长形，2－－插入形
//创建时间：   2006.02.15
//最后修改时间:2005.02.15
//修改原因：
//创建人  ：   刘世杰
//********************************************
int Find_JoinType(int G4x,struct S_Slope slope0,struct S_Slope slope1)
{
    int32_t X0,X1,Y0,Y1,Z0,Z1;
    int   RtNum;
    int   G=slope0.G;
    struct INT64 temp1,temp2,temp3;
    temp1.int_l=0;
    temp1.int_h=0;
    temp2.int_l=0;
    temp2.int_h=0;
    temp3.int_l=0;
    temp3.int_h=0;
    RtNum = 0;
    X0=slope0.Xl;
    X1=slope1.Xl;
    Y0=slope0.Yl;
    Y1=slope1.Yl;
    Z0=slope0.Zl;
    Z1=slope1.Zl;
    if(G4x==0)///左补偿
    {
        if(G==0)
        {
            temp1=sub64(mul64(Y1,X0),mul64(Y0,X1));
            temp2=add64(mul64(Y0,Y1),mul64(X0,X1));
            if((( slope0.length > 0 )&&( slope1.length < 0 ))||( ( slope0.length < 0 )&&( slope1.length > 0 ) ))
            {
                temp1 = sub64(temp3,temp1);
                temp2 = sub64(temp3,temp2);
            }
            if(( temp1.int_h > 0 )||( temp1.int_h == 0 && temp1.int_l > 0))
                RtNum = 0;
            else
            {
                if( temp2.int_h < 0 )
                    RtNum = 2;
            }
            if(( temp1.int_h < 0 )&&( temp2.int_h >= 0 ))
            {
                RtNum = 1;
            }
        }
        if(G==1)
        {
            temp1=sub64(mul64(X1,Z0),mul64(X0,Z1));
            temp2=add64(mul64(X0,X1),mul64(Z0,Z1));
            if((( slope0.length > 0 )&&( slope1.length < 0 ))||( ( slope0.length < 0 )&&( slope1.length > 0 ) ))
            {
                temp1 = sub64(temp3,temp1);
                temp2 = sub64(temp3,temp2);
            }
            if(( temp1.int_h > 0 )||( temp1.int_h == 0 && temp1.int_l > 0))
                RtNum = 0;
            else
            {
                if( temp2.int_h < 0 )
                    RtNum = 2;
            }

            if(( temp1.int_h < 0 )&&( temp2.int_h >= 0 ))
            {
                RtNum = 1;
            }
        }
        if(G==2)
        {
            temp1=sub64(mul64(Z1,Y0),mul64(Z0,Y1));
            temp2=add64(mul64(Z0,Z1),mul64(Y0,Y1));
            if((( slope0.length > 0 )&&( slope1.length < 0 ))||( ( slope0.length < 0 )&&( slope1.length > 0 ) ))
            {
                temp1 = sub64(temp3,temp1);
                temp2 = sub64(temp3,temp2);
            }
            if(( temp1.int_h > 0 )||( temp1.int_h == 0 && temp1.int_l > 0))
                RtNum = 0;
            else
            {
                if( temp2.int_h < 0 )
                    RtNum = 2;
            }

            if(( temp1.int_h < 0 )&&( temp2.int_h >= 0 ))
            {
                RtNum = 1;
            }
        }
    }
    if(G4x==1)///右补偿
    {
        if(G==0)
        {
            temp1=sub64(mul64(Y1,X0),mul64(Y0,X1));
            temp2=add64(mul64(Y0,Y1),mul64(X0,X1));
            if((( slope0.length > 0 )&&( slope1.length < 0 ))||( ( slope0.length < 0 )&&( slope1.length > 0 ) ))
            {
                temp1 = sub64(temp3,temp1);
                temp2 = sub64(temp3,temp2);
            }

            if( temp1.int_h < 0 )
                RtNum = 0;
            else
            {
                if( temp2.int_h < 0 )
                    RtNum = 2;
            }
            if(( temp1.int_h > 0 )||( temp1.int_h ==0 && temp1.int_l > 0 ))
            {
                if( temp2.int_h >= 0 )
                    RtNum = 1;
            }
        }
        if(G==1)
        {
            temp1=sub64(mul64(X1,Z0),mul64(X0,Z1));
            temp2=add64(mul64(X0,X1),mul64(Z0,Z1));
            if((( slope0.length > 0 )&&( slope1.length < 0 ))||( ( slope0.length < 0 )&&( slope1.length > 0 ) ))
            {
                temp1 = sub64(temp3,temp1);
                temp2 = sub64(temp3,temp2);
            }

            if( temp1.int_h < 0 )
                RtNum = 0;
            else
            {
                if( temp2.int_h < 0 )
                    RtNum = 2;
            }
            if(( temp1.int_h > 0 )||( temp1.int_h ==0 && temp1.int_l > 0 ))
            {
                if( temp2.int_h >= 0 )
                    RtNum = 1;
            }
        }
        if(G==2)
        {
            temp1=sub64(mul64(Z1,Y0),mul64(Z0,Y1));
            temp2=add64(mul64(Z0,Z1),mul64(Y0,Y1));
            if((( slope0.length > 0 )&&( slope1.length < 0 ))||( ( slope0.length < 0 )&&( slope1.length > 0 ) ))
            {
                temp1 = sub64(temp3,temp1);
                temp2 = sub64(temp3,temp2);
            }
            if( temp1.int_h < 0 )
                RtNum = 0;
            else
            {
                if( temp2.int_h < 0 )
                    RtNum = 2;
            }
            if(( temp1.int_h > 0 )||( temp1.int_h ==0 && temp1.int_l > 0 ))
            {
                if( temp2.int_h >= 0 )
                    RtNum = 1;
            }
        }
    }
        return  RtNum;
}
//********************************************
//函数功能：   刀具补偿的算法,计算补偿点的坐标
//             计算连接点
//输入参数：   i=0---刀补建立，1－－刀补撤销，2－－刀补进行
//             G＝0－－－G17，1－－G18，2－－G19；circle＝0－－－直线
//             G4x=0 左补偿 ＝1－－右补偿，d－－刀具半径；
//输出参数类型:
//创建时间：   2006.02.15
//最后修改时间:2006.03.01
//修改原因：   进行处理新计算出来的补偿数据,将计算出来的新数据添加到
//             分配好的内存空间中
//创建人  ：   刘世杰
//********************************************
struct RTNDATA Cal_JoinPoint(int i,int G,int G4x,struct S_Line Line1,struct S_Line Line2,int32_t d,char* OutStr)
{
    //平面确定通过CS/BS来确定,不需要额外的变量
    struct  RTNDATA Join_Point;
    struct  RTNDATA Join_Point1;
    struct  RTNDATA Join_Point2;
    struct  RTNDATA Join_Point3;
    struct  RTNDATA Point;
    struct  S_Slope slope1,slope2;
    int32_t   I,J,K,R;
    int32_t   I0,J0,K0;
    int32_t   Xl1,Yl1,Zl1,Xl2,Yl2,Zl2;
    int32_t   X01,Y01,Z01,X02,Y02,Z02;
    int32_t   b,c,f,d1,d2;
    int32_t   Xl,Yl,Zl;
    struct  INT64 tempx1,tempy1,tempz1,tempx2,tempy2,tempz2,temp1,temp2,temp3,temp4,temp5,temp6;
    int     a,m;
    char    G00_1[10],G02_3[10],G17_9[10];
    char    TempChar[100];
    int     Join_Type;       //= 0－－－缩短形，1－－－伸长形，2－－插入形
    int     ConnectLines;    //=0---直线接直线，1--直线接圆弧，2--圆弧接直线，3--圆弧接圆弧
    sprintf(G17_9,"%s","LA");
    a=1;
    m=0;
    for(m=0;m<10;m++)
    {
        G00_1[m]=0;
        G02_3[m]=0;
        G17_9[m]=0;
    }
    //判断转接类型
    if(Line1.type==1||Line1.type==0)
    {
        if(Line2.type==1||0==Line2.type)
             ConnectLines=0;
        else ConnectLines=1;
    }
    else
    {
        if(Line2.type==1||Line2.type==0)
             ConnectLines=2;
        else ConnectLines=3;
    }
    if(G4x==0)
    {
        d=d;
    }
    if(G4x==1)
    {
        d=-d;
    }
    if(Line1.type==0)//G00_1 = "H";
    {
        sprintf(G00_1,"%c",'H');
    }
    if(Line1.type!=0)//G00_1 = "Q";
    {
        sprintf(G00_1,"%c",'Q');
        sprintf(TempChar,"%c%ld",'F',EncodeRegs.CsReg.F);
        strcat(G00_1,TempChar);
    }
    if(G==0)//G17_9 = "LA";
    {
        sprintf(G17_9,"%s","LA");
    }
    if(G==1)//G17_9 = "LB";
    {
        sprintf(G17_9,"%s","LB");
    }
    if(G==2)//G17_9 = "LC";
    {
        sprintf(G17_9,"%s","LC");
    }
//添加平面判断
    if(Line1.type==2)
    {
        sprintf(G02_3,"%c",'M');
        strcat(G02_3,G17_9);
    }
    if(Line1.type==3)
    {
        sprintf(G02_3,"%c",'N');
        strcat(G02_3,G17_9);
    }
    if(ConnectLines==0)//直线接直线
    {
        slope1=CalSlope(G,Line1,0);
        slope2=CalSlope(G,Line2,0);
        Join_Type=Find_JoinType(G4x,slope1,slope2);// Join_Type = 0－－－缩短形，2－－－伸长形，3－－插入形
        tempx1 = div64(mul64(d,slope1.Xl),slope1.length);
        tempy1 = div64(mul64(d,slope1.Yl),slope1.length);
        tempz1 = div64(mul64(d,slope1.Zl),slope1.length);
        tempx2 = div64(mul64(d,slope2.Xl),slope2.length);
        tempy2 = div64(mul64(d,slope2.Yl),slope2.length);
        tempz2 = div64(mul64(d,slope2.Zl),slope2.length);
        Xl1=slope1.Xl;
        Yl1=slope1.Yl;
        Zl1=slope1.Zl;
        Xl2=slope2.Xl;
        Yl2=slope2.Yl;
        Zl2=slope2.Zl;
        if(Join_Type==0)//缩短形
        {
            if(i==0)//刀补建立
            {
                if(G==0)//G17
                {
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);//添加  G01  X Y F
                }
                if(G==1)//G18
                {
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);//添加  G01  X Z F

                }
                if(G==2)//G19
                {
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z);//添加  G01  Y Z F
                }
            }
            if(i==1)//刀补撤销
            {
                if(G==0)//G17
                {
                    Join_Point.y=Line1.Y1+tempx1.int_l;
                    Join_Point.x=Line1.X1-tempy1.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);//添加  G01  X Y F
                }
                if(G==1)//G18
                {
                    Join_Point.x=Line1.X1+tempz1.int_l;
                    Join_Point.z=Line1.Z1-tempx1.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);//添加  G01  X Z F
                }
                if(G==2)//G19
                {
                    Join_Point.z=Line1.Z1+tempy1.int_l;
                    Join_Point.y=Line1.Y1-tempz1.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z);//添加  G01  Y Z F
                }
            }
            if(i==2)//刀补进行
            {
                if(slope1.Xl==slope2.Xl && slope1.Yl==slope2.Yl && slope1.Zl==slope2.Zl)
                {
                    if(G==0)//G17
                    {
                        Join_Point.y=Line1.Y1+tempx1.int_l;
                        Join_Point.x=Line1.X1-tempy1.int_l;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);//添加  G01  X Y F

                    }
                    if(G==1)//G18
                    {
                        Join_Point.x=Line1.X1+tempz1.int_l;
                        Join_Point.z=Line1.Z1-tempx1.int_l;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);//添加  G01  X Z F

                    }
                    if(G==2)//G19
                    {
                        Join_Point.z=Line1.Z1+tempy1.int_l;
                        Join_Point.y=Line1.Y1-tempz1.int_l;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z);//添加  G01  Y Z F
                    }
                }
                else
                {
                    if(G==0)//G17
                    {
                        Point = JointPoint(0,Line1,d,slope1,slope2);
                        Join_Point.x = Point.x;
                        Join_Point.y = Point.y;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);//添加  G01  X Y F
                    }
                    if(G==1)//G18
                    {
                        Point = JointPoint(1,Line1,d,slope1,slope2);
                        Join_Point.x = Point.x;
                        Join_Point.z = Point.z;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);//添加  G01  X Z F

                    }
                    if(G==2)//G19
                    {
                        Point = JointPoint(2,Line1,d,slope1,slope2);
                        Join_Point.z = Point.z;
                        Join_Point.y = Point.y;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z);
                    }
                }
            }
        }
        if(Join_Type==1)//伸长形
        {
            if(i==0)//刀补建立
            {
                if(G==0)//G17
                {
                    Join_Point1.y=Line1.Y1+tempx1.int_l;
                    Join_Point1.x=Line1.X1-tempy1.int_l;
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point.y = Point.y;
                    Join_Point.x = Point.x;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');   //添加  G01   X Y F ; G01 X Y F

                }
                if(G==1)//G18
                {
                    Join_Point1.x=Line1.X1+tempz1.int_l;
                    Join_Point1.z=Line1.Z1-tempx1.int_l;
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point.x = Point.x;
                    Join_Point.z = Point.z;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');   // 添加  G01  X Z F ; G01 X Z F

                }
                if(G==2)//G19
                {
                    Join_Point1.z=Line1.Z1+tempy1.int_l;
                    Join_Point1.y=Line1.Y1-tempz1.int_l;
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point.z = Point.z;
                    Join_Point.y = Point.y;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');  //添加  G01    Y Z F ; G01 Y Z F
                }
            }
            if(i==1)//刀补撤销
            {
                if(G==0)//G17
                {
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point1.y = Point.y;
                    Join_Point1.x = Point.x;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');   //添加  G01   X Y F ; G01 X Y F
                }
                if(G==1)//G18
                {
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point1.x = Point.x;
                    Join_Point1.z = Point.z;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');   //添加  G01   X Z F ; G01 X Z F
                }
                if(G==2)//G19
                {
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point1.z = Point.z;
                    Join_Point1.y = Point.y;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');  //添加  G01    Y Z F ; G01 Y Z F
                }
            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point.y = Point.y;
                    Join_Point.x = Point.x;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);//添加  G01  X Y F
                }
                if(G==1)//G18
                {
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point.x = Point.x;
                    Join_Point.z = Point.z;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);//添加  G01  X Z F
                }
                if(G==2)//G19
                {
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point.z = Point.z;
                    Join_Point.y = Point.y;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z); //添加  G01 Y Z F
                }
            }
        }///end Join_Type==1)
        if(Join_Type==2)//插入形
        {
            temp1 = div64(mul64(labs(d),slope1.Xl),slope1.length);
            temp2 = div64(mul64(labs(d),slope1.Yl),slope1.length);
            temp3 = div64(mul64(labs(d),slope1.Zl),slope1.length);
            temp4 = div64(mul64(labs(d),slope2.Xl),slope1.length);
            temp5 = div64(mul64(labs(d),slope2.Yl),slope1.length);
            temp6 = div64(mul64(labs(d),slope2.Zl),slope1.length);
            if(i==0)//刀补建立
            {
                if(G==0)//G17
                {
                    Join_Point2.y=Line1.Y1+tempx1.int_l;
                    Join_Point2.x=Line1.X1-tempy1.int_l;
                    Join_Point1.y=tempx1.int_l+temp2.int_h+Line1.Y1;
                    Join_Point1.x=(-1)*(int32_t)tempy1.int_l+(int32_t)temp1.int_h+Line1.X1;
                    Join_Point.y=(int32_t)tempx2.int_l-(int32_t)temp5.int_h+Line1.Y1;
                    Join_Point.x=(-1)*(int32_t)tempy2.int_l-(int32_t)temp4.int_h+Line1.X1;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Y',Join_Point2.y,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');  //添加  G01    X Y F;G01   X Y F;G01 X Y F
                }
                if(G==1)//G18
                {
                    Join_Point2.x=Line1.X1+tempz1.int_l;
                    Join_Point2.z=Line1.Z1-tempx1.int_l;
                    Join_Point1.x=tempz1.int_l+temp1.int_h+Line1.X1;
                    Join_Point1.z=(-1)*(int32_t)tempx1.int_l+(int32_t)temp3.int_h+Line1.Z1;
                    Join_Point.x=(int32_t)tempz2.int_l-(int32_t)temp4.int_h+Line1.X1;
                    Join_Point.z=(-1)*(int32_t)tempx2.int_l-(int32_t)temp6.int_h+Line1.Z1;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Z',Join_Point2.z,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');  //添加  G01    X Z F;G01   X Z F;G01 X Z F
                }
                if(G==2)//G19
                {
                    Join_Point2.z=Line1.Z1+tempy1.int_l;
                    Join_Point2.y=Line1.Y1-tempz1.int_l;
                    Join_Point1.z=tempy1.int_l+temp3.int_h +Line1.Z1;
                    Join_Point1.y=(-1)*(int32_t)tempz1.int_l+(int32_t)temp2.int_h+Line1.Y1;
                    Join_Point.z=(int32_t)tempy2.int_l-(int32_t)temp6.int_h+Line1.Z1;
                    Join_Point.y=(-1)*(int32_t)tempz2.int_l-(int32_t)temp5.int_h+Line1.Y1;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point2.y,'Z',Join_Point2.z,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');  //添加  G01    Y Z F;G01   Y Z F;G01 Y Z F
                }
            }
            if(i==1)//刀补撤销
            {
                if(G==0)//G17
                {
                    Join_Point2.y=tempx1.int_l+temp2.int_h+Line1.Y1;
                    Join_Point2.x=(-1)*(int32_t)tempy1.int_l+(int32_t)temp1.int_h+Line1.X1;
                    Join_Point1.y=(int32_t)tempx2.int_l-(int32_t)temp5.int_h+Line1.Y1;
                    Join_Point1.x=(-1)*(int32_t)tempy2.int_l-(int32_t)temp4.int_h+Line1.X1;
                    Join_Point.y=Line1.Y1+(int32_t)tempx1.int_l;
                    Join_Point.x=Line1.X1-(int32_t)tempy1.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Y',Join_Point2.y,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');  //添加  G01    X Y F;G01   X Y F;G01 X Y F
                }
                if(G==1)//G18
                {
                    Join_Point2.x=tempz1.int_l+temp1.int_h+Line1.X1;
                    Join_Point2.z=(-1)*(int32_t)tempx1.int_l+(int32_t)temp3.int_h+Line1.Z1;
                    Join_Point1.x=(int32_t)tempz2.int_l-(int32_t)temp4.int_h+Line1.X1;
                    Join_Point1.z=(-1)*(int32_t)tempx2.int_l-(int32_t)temp6.int_h+Line1.Z1;
                    Join_Point.x=Line1.X1+tempz1.int_l;
                    Join_Point.z=Line1.Z1-tempx1.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Z',Join_Point2.z,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');  //添加  G01    X Z F;G01   X Z F;G01 X Z F
                }
                if(G==2)//G19
                {
                    Join_Point2.z=tempy1.int_l+temp3.int_h+Line1.Z1;
                    Join_Point2.y=(-1)*(int32_t)tempz1.int_l+(int32_t)temp2.int_h+Line1.Y1;
                    Join_Point1.z=(int32_t)tempy2.int_l-(int32_t)temp6.int_h+Line1.Z1;
                    Join_Point1.y=(-1)*(int32_t)tempz2.int_l-(int32_t)temp5.int_h+Line1.Y1;
                    Join_Point.z=Line1.Z1+(int32_t)tempy1.int_l;
                    Join_Point.y=Line1.Y1-(int32_t)tempz1.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point2.y,'Z',Join_Point2.z,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');  //添加  G01    Y Z F;G01   Y Z F;G01 Y Z F
                }
            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    Join_Point1.y=tempx1.int_l+temp2.int_h+Line1.Y1;
                    Join_Point1.x=(-1)*(int32_t)tempy1.int_l+(int32_t)temp1.int_h+Line1.X1;
                    Join_Point.y=(int32_t)tempx2.int_l-(int32_t)temp5.int_h+Line1.Y1;
                    Join_Point.x=(-1)*(int32_t)tempy2.int_l-(int32_t)temp4.int_h+Line1.X1;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';'); //添加  G01 X Y F;G01   X Y F
                }
                if(G==1)//G18
                {
                    Join_Point1.x=(int32_t)tempz1.int_l+(int32_t)temp1.int_h+Line1.X1;
                    Join_Point1.z=(-1)*(int32_t)tempx1.int_l+(int32_t)temp3.int_h+Line1.Z1;
                    Join_Point.x=(int32_t)tempz2.int_l-(int32_t)temp4.int_h+Line1.X1;
                    Join_Point.z=(-1)*(int32_t)tempx2.int_l-(int32_t)temp6.int_h+Line1.Z1;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');  //添加  G01    X Z F;G01   X Z F
                }
                if(G==2)//G19
                {
                    Join_Point1.z=(int32_t)tempy1.int_l+(int32_t)temp3.int_h+Line1.Z1;
                    Join_Point1.y=(-1)*(int32_t)tempz1.int_l+(int32_t)temp2.int_h+Line1.Y1;

                    Join_Point.z=(int32_t)tempy2.int_l-(int32_t)temp6.int_h+Line1.Z1;
                    Join_Point.y=(-1)*(int32_t)tempz2.int_l-(int32_t)temp5.int_h+Line1.Y1;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');  //添加  G01    Y Z F;G01   Y Z F
                }
            }
        }///end Join_Type==2)
    }//end(ConnectLines==0)//直线接直线
    if(ConnectLines==1)//直线接圆弧
    {
        slope1=CalSlope(G,Line1,0);
        slope2=CalSlope(G,Line2,1);
        Join_Type=Find_JoinType(G4x,slope1,slope2); // Join_Type = 0－－－缩短形，2－－－伸长形，3－－插入形
        tempx1 = div64(mul64(d,slope1.Xl),slope1.length);
        tempy1 = div64(mul64(d,slope1.Yl),slope1.length);
        tempz1 = div64(mul64(d,slope1.Zl),slope1.length);
        tempx2 = div64(mul64(d,slope2.Xl),slope2.length);
        tempy2 = div64(mul64(d,slope2.Yl),slope2.length);
        tempz2 = div64(mul64(d,slope2.Zl),slope2.length);
        Xl1=slope1.Xl;
        Yl1=slope1.Yl;
        Zl1=slope1.Zl;
        Xl2=slope2.Xl;
        Yl2=slope2.Yl;
        Zl2=slope2.Zl;
        if(Join_Type==0)//缩短形
        {
            if(i==0)//刀补建立
            {
                if(G==0)//G17
                {
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);   //添加  G01   X Y F
                }
                if(G==1)//G18
                {
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);   //添加  G01   X Z F
                }
                if(G==2)//G19
                {
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z);   //添加  G01   Y Z F
                }
            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    temp1 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
                    if(( temp1.int_h == 0 )&&( temp1.int_l == 0 ))
                    {
                        Join_Point.y=Line1.Y1+tempx2.int_l;
                        Join_Point.x=Line1.X1-tempy2.int_l;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);  //添加  G01    X Y F
                    }
                    else
                    {
                        a=1;
                        if(Line2.type==3) a=-1;
                        I=Line2.I;J=Line2.J;K=Line2.K;
                        R = a*sqr64(add64(mul64(I,I),mul64(J,J)));
                        temp1 = sub64(sub64(mul64(slope1.Xl,J),mul64(slope1.Yl,I)),mul64(d,slope1.length));
                        temp2 = div64(temp1,slope1.length);
                        b = temp2.int_l ;
                        temp1 = div64(add64(mul64(Xl1,I),mul64(Yl1,I)),slope1.length);
                        c = temp1.int_l;
                        f = sqr64(sub64(mul64(R+d,R+d),mul64(b,b)));
                        temp1 = div64X64(mul64X32(add64(mul64(Yl1,J),mul64(Xl1,I)),Yl1),mul64(slope1.length,slope1.length));
                        temp2 = div64(mul64(d,Xl1),slope1.length);
                        temp3 = div64(mul64X32(mul64(c,Yl1),f),slope1.length);
                        Join_Point.y = temp1.int_l+temp2.int_l-temp3.int_l+Line1.Y1;
                        temp1 = div64X64(mul64X32(add64(mul64(Yl1,J),mul64(Xl1,I)),Xl1),mul64(slope1.length,slope1.length));
                        temp2 = div64(mul64(d,Yl1),slope1.length);
                        temp3 = div64(mul64X32(mul64(c,Xl1),f),slope1.length);
                        Join_Point.x = temp1.int_l-temp2.int_l-temp3.int_l+Line1.X1;
                    }
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Y',Join_Point.y);//添加  G01  X Y F
                }
                if(G==1)//G18
                {
                    temp1 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
                    if(( temp1.int_h == 0 )&&( temp1.int_l == 0 ))
                    {
                        Join_Point.x=Line1.X1+tempz2.int_l;
                        Join_Point.z=Line1.Z1-tempx2.int_l;
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);//添加  G01  X Z F
                    }
                    else
                    {
                        a=1;
                        if(Line2.type==3) a=-1;
                        I=Line2.I;J=Line2.J;K=Line2.K;
                        R = a*sqr64(add64(mul64(I,I),mul64(K,K)));
                        temp1 = div64(sub64(sub64(mul64(slope1.Zl,I),mul64(slope1.Xl,K)),mul64(d,slope1.length)),slope1.length);
                        b = temp1.int_l;
                        temp1 = div64(add64(mul64(Zl1,K),mul64(Xl1,I)),slope1.length);
                        c = temp1.int_l;
                        f = sqr64(sub64(mul64(R+d,R+d),mul64(b,b)));
                        temp1 = div64X64(mul64X32(add64(mul64(Xl1,I),mul64(Zl1,K)),Xl1),mul64(slope1.length,slope1.length));
                        temp2 = div64(mul64(d,Zl1),slope1.length);
                        temp3 = div64(mul64X32(mul64(c,Xl1),f),slope1.length);
                        Join_Point.x = temp1.int_l+temp2.int_l-temp3.int_l+Line1.X1;
                        temp1 = div64X64(mul64X32(add64(mul64(Xl1,I),mul64(Zl1,K)),Zl1),mul64(slope1.length,slope1.length));
                        temp2 = div64(mul64(d,Xl1),slope1.length);
                        temp3 = div64(mul64X32(mul64(c,Zl1),f),slope1.length);
                        Join_Point.z=temp1.int_l-temp2.int_l-temp3.int_l+Line1.Z1;
                    }
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'X',Join_Point.x,'Z',Join_Point.z);//添加  G01  X Z F
                }
                if(G==2)//G19
                {
                    //l=slope1.Yl*slope2.Zl-slope2.Yl*slope1.Zl;
                    temp1 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
                    if(( temp1.int_h == 0 )&&( temp1.int_l == 0 ))
                    {
                        Join_Point.z=Line1.Z1+tempy2.int_l;
                        Join_Point.y=Line1.Y1-tempz2.int_l;
                        //添加  G01   Y Z F
                        sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z);

                    }
                    else
                    {
                        a=1;
                        if(Line2.type==3) a=-1;
                        Zl1=slope1.Zl;
                        Yl1=slope1.Yl;
                        I=Line2.I;J=Line2.J;K=Line2.K;
                        //R=a*sqrt(J*J+K*K);
                        R = a*sqr64(add64(mul64(J,J),mul64(K,K)));
                        //b=(Yl1*K-Zl1*J-d);
                        temp1 = div64(sub64(sub64(mul64(slope1.Yl,K),mul64(slope1.Zl,J)),mul64(d,slope1.length)),slope1.length);
                        b = temp1.int_l;
                        //c=Zl1*K+Yl1*J;
                        temp1 = div64(add64(mul64(Zl1,K),mul64(Yl1,I)),slope1.length);
                        c = temp1.int_l;
                        //f=sqrt((R+d)*(R+d)-b*b);
                        f = sqr64(sub64(mul64(R+d,R+d),mul64(b,b)));
                        //Join_Point.z=Zl1*(Zl1*K+Yl1*J)+d*Yl1-c*Zl1*f+Line1.Z1;
                        temp1 = div64X64(mul64X32(add64(mul64(Zl1,K),mul64(Yl1,J)),Zl1),mul64(slope1.length,slope1.length));
                        temp2 = div64(mul64(d,Yl1),slope1.length);
                        temp3 = div64(mul64X32(mul64(c,Zl1),f),slope1.length);
                        Join_Point.z=temp1.int_l+temp2.int_l-temp3.int_l+Line1.Z1;
                        //Join_Point.y=Yl1*(Zl1*K+Yl1*J)-d*Zl1-c*Yl1*f+Line1.Y1;
                        temp1 = div64X64(mul64X32(add64(mul64(Zl1,K),mul64(Yl1,J)),Yl1),mul64(slope1.length,slope1.length));
                        temp2 = div64(mul64(d,Zl1),slope1.length);
                        temp3 = div64(mul64X32(mul64(c,Yl1),f),slope1.length);
                        Join_Point.y=temp1.int_l-temp2.int_l-temp3.int_l+Line1.Y1;

                    }
                    //添加  G01   Y Z F
                    sprintf(TempChar,"%s%c%ld%c%ld",G00_1,'Y',Join_Point.y,'Z',Join_Point.z);

                }
            }
        }//end(Join_Type==0)//缩短形
        if(Join_Type==1)//伸长形
        {

            if(i==0)//刀补建立
            {
                if(G==0)//G17
                {
                    //Join_Point2.y=Line1.Y1+d*Xl1;
                    Join_Point2.y=Line1.Y1+tempx1.int_l;
                    //Join_Point2.x=Line1.X1-d*Yl1;
                    Join_Point2.x=Line1.X1-tempy1.int_l;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.Y1;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point1.y = Point.y;
                    Join_Point1.x = Point.x;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //添加  G01   X Y F;G01   X Y F;G01   X Y F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Y',Join_Point2.y,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point2.x=Line1.X1+d*Zl1;
                    Join_Point2.x=Line1.X1+tempz1.int_l;
                    //Join_Point2.z=Line1.Z1-d*Xl1;
                    Join_Point2.z=Line1.Z1-tempx1.int_l;

                    //Join_Point1.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;

                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point1.x = Point.x;
                    Join_Point1.z = Point.z;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //添加  G01   X Z F;G01   X Z F;G01   X Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Z',Join_Point2.z,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point2.z=Line1.Z1+d*Yl1;
                    Join_Point2.z=Line1.Z1+tempy1.int_l;
                    //Join_Point2.y=Line1.Y1-d*Zl1;
                    Join_Point2.y=Line1.Y1-tempz1.int_l;

                    //Join_Point1.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point1.z = Point.z;
                    Join_Point1.y = Point.y;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    //添加  G01   Y Z F;G01   Y Z F;G01   Y Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point2.y,'Z',Join_Point2.z,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }

            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    //Join_Point1.y=(Yl2-Yl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.Y1;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    //Join_Point.x=Line1.X1-d*Yl2;

                    //Join_Point1.y=(Yl2-Yl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point1.y = Point.y;
                    Join_Point1.x = Point.x;

                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //添加  G01   X Y F;G01   X Y F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point1.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //Join_Point.x=Line1.X1+d*Zl2;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point1.x = Point.x;
                    Join_Point1.z = Point.z;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //添加  G01   X Z F;G01   X Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point1.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //Join_Point.z=Line1.Z1+d*Yl2;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point1.z = Point.z;
                    Join_Point1.y = Point.y;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    //添加  G01   Y Z F;G01   Y Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }

        }//end(Join_Type==1)//伸长形
        if(Join_Type==2)//插入形
        {
            if(i==0)//刀补建立
            {
                if(G==0)//G17
                {
                    //Join_Point3.y=Line1.Y1+d*Xl1;
                    Join_Point3.y=Line1.Y1+tempx1.int_l;
                    //Join_Point3.x=Line1.X1-d*Yl1;
                    Join_Point3.x=Line1.X1-tempy1.int_l;

                    //Join_Point2.y=d*Xl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=tempx1.int_l+temp1.int_l+Line1.Y1;
                    //Join_Point2.x=(-1)*d*Yl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=(-1)*(int32_t)tempy1.int_l+(int32_t)temp1.int_l+Line1.X1;

                    //Join_Point1.y=d*Xl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=tempx2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point1.x=(-1)*d*Yl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=(-1)*(int32_t)tempy2.int_l-(int32_t)temp1.int_l+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //str_mid.Format(_T("(;%sY%.0fX%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;)"),
                    //             G00_1,Join_Point3.y*1000,Join_Point3.x*1000,
                    //             G00_1,Join_Point2.y*1000,Join_Point2.x*1000,
                    //             G00_1,Join_Point1.y*1000,Join_Point1.x*1000,
                    //             G00_1,Join_Point.y*1000,Join_Point.x*1000);
                    //添加  G01   X Y F;G01   X Y F;G01   X Y F;G01   X Y F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point3.x,'Y',Join_Point3.y,';',G00_1,'X',Join_Point2.x,'Y',Join_Point2.y,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point3.x=Line1.X1+d*Zl1;
                    Join_Point3.x=Line1.X1+tempz1.int_l;
                    //Join_Point3.z=Line1.Z1-d*Xl1;
                    Join_Point3.z=Line1.Z1-tempx1.int_l;
                    //Join_Point2.x=d*Zl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=tempz1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point2.z=(-1)*d*Xl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=(-1)*(int32_t)tempx1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point1.x=d*Zl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=tempz2.int_l-temp1.int_l+Line1.X1;

                    //Join_Point1.z=(-1)*d*Xl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=(-1)*(int32_t)tempx2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //str_mid.Format(_T("(;%sX%.0fZ%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;)"),
                    //             G00_1,Join_Point3.x*1000,Join_Point3.z*1000,
                    //             G00_1,Join_Point2.x*1000,Join_Point2.z*1000,
                    //             G00_1,Join_Point1.x*1000,Join_Point1.z*1000,
                    //             G00_1,Join_Point.x*1000,Join_Point.z*1000);
                    //添加  G01   X Z F;G01   X Z F;G01   X Z F;G01   X Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point3.x,'Z',Join_Point3.z,';',G00_1,'X',Join_Point2.x,'Z',Join_Point2.z,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point3.z=Line1.Z1+d*Yl1;
                    Join_Point3.z=Line1.Z1+tempy1.int_l;
                    //Join_Point3.y=Line1.Y1-d*Zl1;
                    Join_Point3.y=Line1.Y1-tempz1.int_l;
                    //Join_Point2.z=d*Yl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=tempy1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point2.y=(-1)*d*Zl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=(-1)*(int32_t)tempz1.int_l+temp1.int_l+Line1.Y1;

                    //Join_Point1.z=d*Yl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=tempy2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point1.y=(-1)*d*Zl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=(-1)*(int32_t)tempz2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    //str_mid.Format(_T("(;%sZ%.0fY%.0f;%sZ%.0fY%.0f;%sZ%.0fY%.0f;%sZ%.0fY%.0f;)"),
                    //             G00_1,Join_Point3.z*1000,Join_Point3.y*1000,
                    //             G00_1,Join_Point2.z*1000,Join_Point2.y*1000,
                    //             G00_1,Join_Point1.z*1000,Join_Point1.y*1000,
                    //             G00_1,Join_Point.z*1000,Join_Point.y*1000);
                    //添加  G01   Y Z F;G01   Y Z F;G01   Y Z F;G01   Y Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point3.y,'Z',Join_Point3.z,';',G00_1,'Y',Join_Point2.y,'Z',Join_Point2.z,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    //Join_Point2.y=d*Xl1+abs(d)*Yl1+Line1.Y1;
                    //Join_Point2.x=(-1)*d*Yl1+abs(d)*Xl1+Line1.X1;
                    //Join_Point1.y=d*Xl2-abs(d)*Yl2+Line1.Y1;
                    //Join_Point1.x=(-1)*d*Yl2-abs(d)*Xl2+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    //Join_Point2.y=d*Xl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=tempx1.int_l+temp1.int_l+Line1.Y1;
                    //Join_Point2.x=(-1)*d*Yl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=(-1)*(int32_t)tempy1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point1.y=d*Xl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=tempx2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point1.x=(-1)*d*Yl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=(-1)*(int32_t)tempy2.int_l-temp1.int_l+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //str_mid.Format(_T("(;%sY%.0fX%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;)"),
                    //             G00_1,Join_Point2.y*1000,Join_Point2.x*1000,
                    //             G00_1,Join_Point1.y*1000,Join_Point1.x*1000,
                    //             G00_1,Join_Point.y*1000,Join_Point.x*1000);
                    //添加  G01   X Y F;G01   X Y F;G01   X Y F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Y',Join_Point2.y,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point2.x=d*Zl1+abs(d)*Xl1+Line1.X1;
                    //Join_Point2.z=(-1)*d*Xl1+abs(d)*Zl1+Line1.Z1;
                    //Join_Point1.x=d*Zl2-abs(d)*Xl2+Line1.X1;
                    //Join_Point1.z=(-1)*d*Xl2-abs(d)*Zl2+Line1.Z1;
                    //Join_Point.x=Line1.X1+d*Zl2;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    //Join_Point2.x=d*Zl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=tempz1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point2.z=(-1)*d*Xl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=(-1)*(int32_t)tempx1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point1.x=d*Zl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=tempz2.int_l-temp1.int_l+Line1.X1;

                    //Join_Point1.z=(-1)*d*Xl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=(-1)*(int32_t)tempx2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //str_mid.Format(_T("(;%sX%.0fZ%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;)"),
                    //             G00_1,Join_Point2.x*1000,Join_Point2.z*1000,
                    //             G00_1,Join_Point1.x*1000,Join_Point1.z*1000,
                    //             G00_1,Join_Point.x*1000,Join_Point.z*1000);
                    //添加  G01   X Z F;G01   X Z F;G01   X Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point2.x,'Z',Join_Point2.z,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point2.z=d*Yl1+abs(d)*Zl1+Line1.Z1;
                    //Join_Point2.y=(-1)*d*Zl1+abs(d)*Yl1+Line1.Y1;
                    //Join_Point1.z=d*Yl2-abs(d)*Zl2+Line1.Z1;
                    //Join_Point1.y=(-1)*d*Zl2-abs(d)*Yl2+Line1.Y1;
                    //Join_Point.z=Line1.Z1+d*Yl2;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    //Join_Point2.z=d*Yl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=tempy1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point2.y=(-1)*d*Zl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=(-1)*(int32_t)tempz1.int_l+temp1.int_l+Line1.Y1;

                    //Join_Point1.z=d*Yl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=tempy2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point1.y=(-1)*d*Zl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=(-1)*(int32_t)tempz2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    //str_mid.Format(_T("(;%sZ%.0fY%.0f;%sZ%.0fY%.0f;%sZ%.0fY%.0f;)"),
                    //             G00_1,Join_Point2.z*1000,Join_Point2.y*1000,
                    //             G00_1,Join_Point1.z*1000,Join_Point1.y*1000,
                    //             G00_1,Join_Point.z*1000,Join_Point.y*1000);
                    //添加  G01   Y Z F;G01   Y Z F;G01   Y Z F
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point2.y,'Z',Join_Point2.z,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }

        }//end(Join_Type==1)//插入形
    }//end(ConnectLines==1)//直线接圆弧

    if(ConnectLines==2)//圆弧接直线
    {
        slope1=CalSlope(G,Line1,2);
        slope2=CalSlope(G,Line2,0);
        // Join_Type = 0－－－缩短形，2－－－伸长形，3－－插入形
        Join_Type=Find_JoinType(G4x,slope1,slope2);
        tempx1 = div64(mul64(d,slope1.Xl),slope1.length);
        tempy1 = div64(mul64(d,slope1.Yl),slope1.length);
        tempz1 = div64(mul64(d,slope1.Zl),slope1.length);
        tempx2 = div64(mul64(d,slope2.Xl),slope2.length);
        tempy2 = div64(mul64(d,slope2.Yl),slope2.length);
        tempz2 = div64(mul64(d,slope2.Zl),slope2.length);
        Xl1=slope1.Xl;
        Yl1=slope1.Yl;
        Zl1=slope1.Zl;

        Xl2=slope2.Xl;
        Yl2=slope2.Yl;
        Zl2=slope2.Zl;
        I=Line1.I;J=Line1.J;K=Line1.K;

        I0=I+Line1.X0-DBRegs.DB_Start_Point.x;
        J0=J+Line1.Y0-DBRegs.DB_Start_Point.y;
        K0=K+Line1.Z0-DBRegs.DB_Start_Point.z;


        if(Join_Type==0)//缩短形
        {

            if(i==1)//刀补撤销
            {
                if(G==0)//G17
                {

                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //str_mid.Format(_T("%sX%.0fY%.0fI%.0fJ%.0fF%.0f"),G02_3,Join_Point.x*1000,Join_Point.y*1000,I0*1000,J0*1000,vel*2/15);
                    //
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'X',Join_Point.x,'Y',Join_Point.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F);

                }
                if(G==1)//G18
                {
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //str_mid.Format(_T("%sX%.0fZ%.0fI%.0fK%.0fF%.0f"),G02_3,Join_Point.x*1000,Join_Point.z*1000,I0*1000,K0*1000,vel*2/15);
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'X',Join_Point.x,'Z',Join_Point.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F);

                }
                if(G==2)//G19
                {
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    //str_mid.Format(_T("%sY%.0fZ%.0fJ%.0fK%.0fF%.0f"),G02_3,Join_Point.y*1000,Join_Point.z*1000,J0*1000,K0*1000,vel*2/15);
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'Y',Join_Point.y,'Z',Join_Point.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F);

                }
            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    //l=slope1.Xl*slope2.Yl-slope2.Xl*slope1.Yl;
                    temp1 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
                    if(( temp1.int_h == 0 )&&( temp1.int_l == 0 ))
                    {
                        Join_Point.y=Line1.Y1+tempx2.int_l;
                        Join_Point.x=Line1.X1-tempy2.int_l;
                    }
                    else
                    {
                        a=1;
                        if(Line2.type==3) a=-1;
                        //R=a*sqrt(I*I+J*J);
                        R = a*sqr64(add64(mul64(I,I),mul64(J,J)));

                        Y01=Line1.Y0+J-Line1.Y1;
                        X01=Line1.X0+I-Line1.X1;

                        //b=(Xl2*Y01-Yl2*X01-d);
                        temp1 = sub64(sub64(mul64(Xl2,Y01),mul64(Yl2,X01)),mul64(d,slope2.length));
                        temp2 = div64(temp1,slope2.length);
                        b = temp2.int_l ;

                        //c=(Xl2*X01+Yl2*Y01);
                        temp1 = div64(add64(mul64(Xl2,X01),mul64(Yl2,Y01)),slope2.length);
                        c = temp1.int_l;
                        //f=sqrt((R+d)*(R+d)-b*b);
                        f = sqr64(sub64(mul64(R+d,R+d),mul64(b,b)));
                        //Join_Point.y=Yl2*(Xl2*X01+Yl2*Y01)+d*Xl2-c*Yl2*f+Line1.Y1;
                        temp1 = div64X64(mul64X32(add64(mul64(Xl2,X01),mul64(Yl2,Y01)),Yl2),mul64(slope2.length,slope2.length));
                        temp2 = div64(mul64(d,Xl2),slope2.length);
                        temp3 = div64(mul64X32(mul64(c,Yl2),f),slope2.length);
                        Join_Point.y = temp1.int_l+temp2.int_l-temp3.int_l+Line1.Y1;

                        //Join_Point.x=Xl2*(Xl2*X01+Yl2*Y01)-d*Yl2-c*Xl1*f+Line1.X1;
                        temp1 = div64X64(mul64X32(add64(mul64(Xl2,X01),mul64(Yl2,Y01)),Xl2),mul64(slope2.length,slope2.length));
                        temp2 = div64(mul64(d,Yl2),slope2.length);
                        temp3 = div64(mul64X32(mul64(c,Xl1),f),slope1.length);
                        Join_Point.x = temp1.int_l-temp2.int_l-temp3.int_l+Line1.X1;
                    }
                    //str_mid.Format(_T("%sY%.0fX%.0fI%.0fJ%.0fF%.0f"),G02_3,Join_Point.y*1000,Join_Point.x*1000,I0*1000,J0*1000,vel*2/15);
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'X',Join_Point.x,'Y',Join_Point.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F);

                }
                if(G==1)//G18
                {
                    //l=slope1.Zl*slope2.Xl-slope2.Zl*slope1.Xl;
                    temp1 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
                    if(( temp1.int_h == 0 )&&( temp1.int_l == 0 ))
                    {
                        Join_Point.x=Line1.X1+tempz2.int_l;
                        Join_Point.z=Line1.Z1-tempx2.int_l;
                    }
                    else
                    {
                        a=1;
                        if(Line2.type==3) a=-1;
                        //R=a*sqrt(I*I+K*K);
                        R = a*sqr64(add64(mul64(I,I),mul64(K,K)));

                        X01=Line1.X0+I-Line1.X1;
                        Z01=Line1.Z0+K-Line1.Z1;

                        //b=(Zl2*X01-Xl2*Z01-d);
                        temp1 = sub64(sub64(mul64(Zl2,X01),mul64(Xl2,Z01)),mul64(d,slope2.length));
                        temp2 = div64(temp1,slope2.length);
                        b = temp2.int_l ;

                        //c=(Zl2*Z01+Xl2*X01);
                        temp1 = div64(add64(mul64(Zl2,Z01),mul64(Xl2,X01)),slope2.length);
                        c = temp1.int_l;
                        //f=sqrt((R+d)*(R+d)-b*b);
                        f = sqr64(sub64(mul64(R+d,R+d),mul64(b,b)));

                        //Join_Point.x=Xl2*(Zl2*Z01+Xl2*X01)+d*Zl2-c*Xl2*f+Line1.X1;
                        temp1 = div64X64(mul64X32(add64(mul64(Zl2,Z01),mul64(Xl2,X01)),Xl2),mul64(slope2.length,slope2.length));
                        temp2 = div64(mul64(d,Zl2),slope2.length);
                        temp3 = div64(mul64X32(mul64(c,Xl2),f),slope2.length);
                        Join_Point.x = temp1.int_l+temp2.int_l-temp3.int_l+Line1.X1;

                        //Join_Point.z=Zl2*(Zl2*Z01+Xl2*X01)-d*Xl2-c*Zl1*f+Line1.Z1;
                        temp1 = div64X64(mul64X32(add64(mul64(Zl2,Z01),mul64(Xl2,X01)),Zl2),mul64(slope2.length,slope2.length));
                        temp2 = div64(mul64(d,Xl2),slope2.length);
                        temp3 = div64(mul64X32(mul64(c,Zl1),f),slope1.length);
                        Join_Point.z = temp1.int_l-temp2.int_l-temp3.int_l+Line1.Z1;
                    }
                    //str_mid.Format(_T("%sX%.0fZ%.0fI%.0fK%.0fF%.0f"),G02_3,Join_Point.x*1000,Join_Point.z*1000,I0*1000,K0*1000,vel*2/15);
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'X',Join_Point.x,'Z',Join_Point.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F);

                }
                if(G==2)//G19
                {
                    //l=slope1.Yl*slope2.Zl-slope2.Yl*slope1.Zl;
                    temp1 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
                    if(( temp1.int_h == 0 )&&( temp1.int_l == 0 ))
                    {
                        Join_Point.z=Line1.Z1+tempy2.int_l;
                        Join_Point.y=Line1.Y1-tempz2.int_l;
                    }
                    else
                    {
                        a=1;
                        if(Line2.type==3) a=-1;
                        //R=a*sqrt(J*J+K*K);
                        R = a*sqr64(add64(mul64(J,J),mul64(K,K)));

                        Z01=Line1.Z0+K-Line1.Z1;
                        Y01=Line1.Y0+J-Line1.Y1;

                        //b=(Yl2*Z01-Zl2*Y01-d);
                        temp1 = sub64(sub64(mul64(Yl2,Z01),mul64(Zl2,Y01)),mul64(d,slope2.length));
                        temp2 = div64(temp1,slope2.length);
                        b = temp2.int_l ;

                        //c=(Yl2*Y01+Zl2*Z01);
                        temp1 = div64(add64(mul64(Yl2,Y01),mul64(Zl2,Z01)),slope2.length);
                        c = temp1.int_l;

                        //f=sqrt((R+d)*(R+d)-b*b);
                        f = sqr64(sub64(mul64(R+d,R+d),mul64(b,b)));

                        //Join_Point.z=Zl2*(Yl2*Y01+Zl2*Z01)+d*Yl2-c*Zl2*f+Line1.Z1;
                        temp1 = div64X64(mul64X32(add64(mul64(Yl2,Y01),mul64(Zl2,Z01)),Zl2),mul64(slope2.length,slope2.length));
                        temp2 = div64(mul64(d,Yl2),slope2.length);
                        temp3 = div64(mul64X32(mul64(c,Zl2),f),slope2.length);
                        Join_Point.z = temp1.int_l+temp2.int_l-temp3.int_l+Line1.Z1;

                        //Join_Point.y=Yl2*(Yl2*Y01+Zl2*Z01)-d*Zl2-c*Yl1*f+Line1.Y1;
                        temp1 = div64X64(mul64X32(add64(mul64(Yl2,Y01),mul64(Zl2,Z01)),Yl2),mul64(slope2.length,slope2.length));
                        temp2 = div64(mul64(d,Zl2),slope2.length);
                        temp3 = div64(mul64X32(mul64(c,Yl1),f),slope1.length);
                        Join_Point.y= temp1.int_l-temp2.int_l-temp3.int_l+Line1.Y1;
                    }
                    //str_mid.Format(_T("%sZ%.0fY%.0fJ%.0fK%.0fF%.0f"),G02_3,Join_Point.z*1000,Join_Point.y*1000,J0*1000,K0*1000,vel*2/15);
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'Y',Join_Point.y,'Z',Join_Point.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F);

                }
            }
        }//end(Join_Type==0)//缩短形
        if(Join_Type==1)//伸长形
        {
            if(i==1)//刀补撤销
            {
                if(G==0)//G17
                {
                    //Join_Point2.y=Line1.Y1+d*Xl1;
                    Join_Point2.y=Line1.Y1+tempx1.int_l;
                    //Join_Point2.x=Line1.X1-d*Yl1;
                    Join_Point2.x=Line1.X1-tempy1.int_l;

                    //Join_Point1.y=(Yl2-Yl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point1.y = Point.y;
                    Join_Point1.x = Point.x;

                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //str_mid.Format(_T("(;%sY%.0fX%.0fI%.0fK%.0fF%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;)"),
                    //             G02_3,Join_Point2.y*1000,Join_Point2.x*1000,I0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point1.y*1000,Join_Point1.x*1000,
                    //             G00_1,Join_Point.y*1000,Join_Point.x*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point2.x,'Y',Join_Point2.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point2.x=Line1.X1+d*Zl1;
                    Join_Point2.x=Line1.X1+tempz1.int_l;
                    //Join_Point2.z=Line1.Z1-d*Xl1;
                    Join_Point2.z=Line1.Z1-tempx1.int_l;

                    //Join_Point1.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //Join_Point.x=Line1.X1+d*Zl2;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point1.x = Point.x;
                    Join_Point1.z = Point.z;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point2.x,'Z',Join_Point2.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point2.z=Line1.Z1+d*Yl1;
                    Join_Point2.z=Line1.Z1+tempy1.int_l;
                    //Join_Point2.y=Line1.Y1-d*Zl1;
                    Join_Point2.y=Line1.Y1-tempz1.int_l;

                    //Join_Point1.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //Join_Point.z=Line1.Z1+d*Yl2;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point1.z = Point.z;
                    Join_Point1.y = Point.y;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'Y',Join_Point2.y,'Z',Join_Point2.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    //Join_Point1.y=Line1.Y1+d*Xl2;
                    Join_Point1.y=Line1.Y1+tempx2.int_l;
                    //Join_Point1.x=Line1.X1-d*Yl2;
                    Join_Point1.x=Line1.X1-tempy2.int_l;

                    //Join_Point.y=(Yl2-Yl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point.y=temp3.int_l+Line1.Y1;

                    //Join_Point.x=(Xl2-Xl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point.x=temp3.int_l+Line1.X1;
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point.y = Point.y;
                    Join_Point.x = Point.x;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point1.x,'Y',Join_Point1.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point1.x=Line1.X1+d*Zl2;
                    Join_Point1.x=Line1.X1+tempz2.int_l;
                    //Join_Point1.z=Line1.Z1-d*Xl2;
                    Join_Point1.z=Line1.Z1-tempx2.int_l;

                    //Join_Point.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point.x=temp3.int_l+Line1.X1;
                    //Join_Point.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point.z=temp3.int_l+Line1.Z1;
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point.x = Point.x;
                    Join_Point.z = Point.z;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point1.x,'Z',Join_Point1.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==2)//G19
                {
                    //Join_Point1.z=Line1.Z1+d*Yl2;
                    Join_Point1.z=Line1.Z1+tempy2.int_l;
                    //Join_Point1.y=Line1.Y1-d*Zl2;
                    Join_Point1.y=Line1.Y1-tempz2.int_l;

                    //Join_Point.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point.z=temp3.int_l+Line1.Z1;
                    //Join_Point.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point.y=temp3.int_l+Line1.Y1;
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point.z = Point.z;
                    Join_Point.y = Point.y;
                    //str_mid.Format(_T("(;%sZ%.0fY%.0fJ%.0fK%.0fF%.0f;%sZ%.0fY%.0f;)"),
                    //             G02_3,Join_Point1.z*1000,Join_Point1.y*1000,J0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point.z*1000,Join_Point.y*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'Y',Join_Point1.y,'Z',Join_Point1.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }

        }//end(Join_Type==1)//伸长形
        if(Join_Type==2)//插入形
        {
            if(i==1)//刀补撤销
            {
                if(G==0)//G17
                {
                    //Join_Point3.y=Line1.Y1+d*Xl1;
                    Join_Point3.y=Line1.Y1+tempx1.int_l;
                    //Join_Point3.x=Line1.X1-d*Yl1;
                    Join_Point3.x=Line1.X1-tempy1.int_l;
                    //Join_Point2.y=d*Xl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=tempx1.int_l+temp1.int_l+Line1.Y1;
                    //Join_Point2.x=(-1)*d*Yl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=(-1)*(int32_t)tempy1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point1.y=d*Xl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=tempx2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point1.x=(-1)*d*Yl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=(-1)*(int32_t)tempy2.int_l-temp1.int_l+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;

                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point3.x,'Y',Join_Point3.y,';',G00_1,'X',Join_Point2.x,'Y',Join_Point2.y,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point3.x=Line1.X1+d*Zl1;
                    Join_Point3.x=Line1.X1+tempz1.int_l;
                    //Join_Point3.z=Line1.Z1-d*Xl1;
                    Join_Point3.z=Line1.Z1-tempx1.int_l;

                    //Join_Point2.x=d*Zl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=tempz1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point2.z=(-1)*d*Xl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=(-1)*(int32_t)tempx1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point1.x=d*Zl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=tempz2.int_l-temp1.int_l+Line1.X1;

                    //Join_Point1.z=(-1)*d*Xl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=(-1)*(int32_t)tempx2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //str_mid.Format(_T("(;%sX%.0fZ%.0fI%.0fK%.0fF%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;)"),
                    //             G02_3,Join_Point3.x*1000,Join_Point3.z*1000,I0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point2.x*1000,Join_Point2.z*1000,
                    //             G00_1,Join_Point1.x*1000,Join_Point1.z*1000,
                    //             G00_1,Join_Point.x*1000,Join_Point.z*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'X',Join_Point3.x,'Z',Join_Point3.z,';',G00_1,'X',Join_Point2.x,'Z',Join_Point2.z,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(2==G)//G19
                {
                    //Join_Point3.z=Line1.Z1+d*Yl1;
                    Join_Point3.z=Line1.Z1+tempy1.int_l;
                    //Join_Point3.y=Line1.Y1-d*Zl1;
                    Join_Point3.y=Line1.Y1-tempz1.int_l;
                    //Join_Point2.z=d*Yl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=tempy1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point2.y=(-1)*d*Zl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=(-1)*(int32_t)tempz1.int_l+temp1.int_l+Line1.Y1;

                    //Join_Point1.z=d*Yl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=tempy2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point1.y=(-1)*d*Zl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=(-1)*(int32_t)tempz2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G00_1,'Y',Join_Point3.y,'Z',Join_Point3.z,';',G00_1,'Y',Join_Point2.y,'Z',Join_Point2.z,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    //Join_Point2.y=Line1.Y1+d*Xl1;
                    Join_Point2.y=Line1.Y1+tempx1.int_l;
                    //Join_Point2.x=Line1.X1-d*Yl1;
                    Join_Point2.x=Line1.X1-tempy1.int_l;
                    //Join_Point1.y=d*Xl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point1.y=tempx1.int_l+temp1.int_l+Line1.Y1;
                    //Join_Point1.x=(-1)*d*Yl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point1.x=(-1)*(int32_t)tempy1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point.y=d*Xl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point.y=tempx2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point.x=(-1)*d*Yl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point.x=(-1)*(int32_t)tempy2.int_l-temp1.int_l+Line1.X1;

                    //str_mid.Format(_T("(;%sY%.0fX%.0fI%.0fJ%.0fF%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;)"),
                    //             G02_3,Join_Point2.y*1000,Join_Point2.x*1000,I0*1000,J0*1000,vel*2/15,
                    //             G00_1,Join_Point1.y*1000,Join_Point1.x*1000,
                    //             G00_1,Join_Point.y*1000,Join_Point.x*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point2.x,'Y',Join_Point2.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point2.x=Line1.X1+d*Zl1;
                    Join_Point2.x=Line1.X1+tempz1.int_l;
                    //Join_Point2.z=Line1.Z1-d*Xl1;
                    Join_Point2.z=Line1.Z1-tempx1.int_l;


                    //Join_Point1.x=d*Zl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point1.x=tempz1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point1.z=(-1)*d*Xl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point1.z=(-1)*(int32_t)tempx1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point.x=d*Zl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point.x=tempz2.int_l-temp1.int_l+Line1.X1;

                    //Join_Point.z=(-1)*d*Xl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point.z=(-1)*(int32_t)tempx2.int_l-temp1.int_l+Line1.Z1;

                    //str_mid.Format(_T("(;%sX%.0fZ%.0fI%.0fK%.0fF%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;)"),
                    //             G02_3,Join_Point2.x*1000,Join_Point2.z*1000,I0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point1.x*1000,Join_Point1.z*1000,
                    //             G00_1,Join_Point.x*1000,Join_Point.z*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point2.x,'Z',Join_Point2.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point2.z=Line1.Z1+d*Yl1;
                    Join_Point2.z=Line1.Z1+tempy1.int_l;
                    //Join_Point2.y=Line1.Y1-d*Zl1;
                    Join_Point2.y=Line1.Y1-tempz1.int_l;

                    //Join_Point1.z=d*Yl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point1.z=tempy1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point1.y=(-1)*d*Zl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point1.y=(-1)*(int32_t)tempz1.int_l+temp1.int_l+Line1.Y1;

                    //Join_Point.z=d*Yl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point.z=tempy2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point.y=(-1)*d*Zl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point.y=(-1)*(int32_t)tempz2.int_l-temp1.int_l+Line1.Y1;

                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'Y',Join_Point2.y,'Z',Join_Point2.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }

        }//end(Join_Type==1)//插入形
    }//end(ConnectLines==2)//圆弧接直线


    if(ConnectLines==3)//圆弧接圆弧
    {
        slope1=CalSlope(G,Line1,2);
        slope2=CalSlope(G,Line2,1);
        // Join_Type = 0－－－缩短形，2－－－伸长形，3－－插入形
        Join_Type=Find_JoinType(G4x,slope1,slope2);
        tempx1 = div64(mul64(d,slope1.Xl),slope1.length);
        tempy1 = div64(mul64(d,slope1.Yl),slope1.length);
        tempz1 = div64(mul64(d,slope1.Zl),slope1.length);
        tempx2 = div64(mul64(d,slope2.Xl),slope2.length);
        tempy2 = div64(mul64(d,slope2.Yl),slope2.length);
        tempz2 = div64(mul64(d,slope2.Zl),slope2.length);
        Xl1=slope1.Xl;
        Yl1=slope1.Yl;
        Zl1=slope1.Zl;

        Xl2=slope2.Xl;
        Yl2=slope2.Yl;
        Zl2=slope2.Zl;
        I=Line1.I;J=Line1.J;K=Line1.K;

        I0=I+Line1.X0-DBRegs.DB_Start_Point.x;
        J0=J+Line1.Y0-DBRegs.DB_Start_Point.y;
        K0=K+Line1.Z0-DBRegs.DB_Start_Point.z;

        if(Join_Type==0)//缩短形
        {

            if(i==2)//刀补进行
            {
                if(0==G)//G17
                {
                    int32_t R1,R2,I1,J1,I2,J2;//K1,K2;
                    int a=1;
                    if(Line2.type==3) a=-1;
                    I1=Line1.I;
                    J1=Line1.J;//K1=Line1.K;
                    I2=Line2.I;
                    J2=Line2.J;//K2=Line2.K;
                    //R1=a*sqrt(I1*I1+J1*J1);
                    R1=a*sqr64(add64(mul64(I1,I1),mul64(J1,J1)));
                    //R2=a*sqrt(I2*I2+J2*J2);
                    R2=a*sqr64(add64(mul64(I2,I2),mul64(J2,J2)));
                    Y01=Line1.Y0+J1-Line1.Y1;
                    X01=Line1.X0+I1-Line1.X1;
                    Y02=J2;
                    X02=I2;

                    //d1=sqrt((Y02-Y01)*(Y02-Y01)+(X02-X01)*(X02-X01));
                    d1=sqr64(add64(mul64(Y02-Y01,Y02-Y01),mul64(X02-X01,X02-X01)));;
                    //d2=d*(R1-R2)/d1;
                    temp1=div64(mul64(d,R1-R2),d1);
                    d2 = temp1.int_l;

                    //Yl=(-1)*(X02-X01)/d1;
                    Yl=(-1)*(X02-X01);
                    //Xl=(Y02-Y01)/d1;
                    Xl=Y02-Y01;

                    //b=(Xl*Y01-Yl*X01-d2);
                    temp1=div64(sub64(sub64(mul64(Xl,Y01),mul64(Yl,X01)),mul64(d1,d2)),d1);
                    b = temp1.int_l;
                    //c=(Xl*X01+Yl*Y01);
                    temp1 = div64(add64(mul64(Xl,X01),mul64(Yl,Y01)),d1);
                    c = temp1.int_l;

                    //f=sqrt((R1+d)*(R1+d)-b*b);
                    f=sqr64(sub64(mul64(R1+d,R1+d),mul64(b,b)));
                    //Join_Point.y=Yl*(Xl*X01+Yl*Y01)+d2*Xl-c*Yl*f+Line1.Y1;
                    temp1 = div64X64(mul64X32(add64(mul64(Xl,X01),mul64(Yl,Y01)),Yl),mul64(d1,d1));
                    temp2 = div64(mul64(d2,Xl),d1);
                    temp3 = div64(mul64X32(mul64(c,Yl),f),d1);
                    Join_Point.y = temp1.int_l+temp2.int_l-temp3.int_l+Line1.Y1;

                    //Join_Point.x=Xl*(Xl*X01+Yl*Y01)-d2*Yl-c*Xl*f+Line1.X1;
                    temp1 = div64X64(mul64X32(add64(mul64(Xl,X01),mul64(Yl,Y01)),Xl),mul64(d1,d1));
                    temp2 = div64(mul64(d2,Yl),d1);
                    temp3 = div64(mul64X32(mul64(c,Xl),f),d1);
                    Join_Point.x = temp1.int_l-temp2.int_l-temp3.int_l+Line1.X1;
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'X',Join_Point.x,'Y',Join_Point.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F);


                }
                if(1==G)//G18
                {

                    int32_t R1,R2,I1,K1,I2,K2;// J1,,J2
                    int a=1;
                    if(Line2.type==3) a=-1;
                    I1=Line1.I;//   J1=Line1.J;
                    K1=Line1.K;
                    I2=Line2.I; //J2=Line2.J;
                    K2=Line2.K;
                    //R1=a*sqrt(I1*I1+K1*K1);
                    R1=a*sqr64(add64(mul64(I1,I1),mul64(K1,K1)));
                    //R2=a*sqrt(I2*I2+K2*K2);
                    R2=a*sqr64(add64(mul64(I2,I2),mul64(K2,K2)));

                    X01=Line1.X0+I1-Line1.X1;
                    Z01=Line1.Z0+K1-Line1.Z1;
                    X02=I2;
                    Z02=K2;

                    //d1=sqrt((X02-X01)*(X02-X01)+(Z02-Z01)*(Z02-Z01));
                    d1=sqr64(add64(mul64(X02-X01,X02-X01),mul64(Z02-Z01,Z02-Z01)));;

                    //d2=d*(R1-R2)/d1;
                    temp1=div64(mul64(d,R1-R2),d1);
                    d2 = temp1.int_l;

                    //Xl=(-1)*(Z02-Z01)/d1;
                    Xl=(-1)*(Z02-Z01);
                    //Zl=(X02-X01)/d1;
                    Zl=X02-X01;

                    //b=(Zl*X01-Xl*Z01-d2);
                    temp1=div64(sub64(sub64(mul64(Zl,X01),mul64(Xl,Z01)),mul64(d1,d2)),d1);
                    b = temp1.int_l;

                    //c=(Zl*Z01+Xl*X01);
                    temp1 = div64(add64(mul64(Zl,Z01),mul64(Xl,X01)),d1);
                    c = temp1.int_l;

                    //f=sqrt((R1+d)*(R1+d)-b*b);
                    f=sqr64(sub64(mul64(R1+d,R1+d),mul64(b,b)));

                    //Join_Point.x=Xl*(Zl*Z01+Xl*X01)+d2*Zl-c*Xl*f+Line1.X1;
                    temp1 = div64X64(mul64X32(add64(mul64(Zl,Z01),mul64(Xl,X01)),Xl),mul64(d1,d1));
                    temp2 = div64(mul64(d2,Zl),d1);
                    temp3 = div64(mul64X32(mul64(c,Xl),f),d1);
                    Join_Point.x = temp1.int_l+temp2.int_l-temp3.int_l+Line1.X1;

                    Join_Point.z=Zl*(Zl*Z01+Xl*X01)-d2*Xl-c*Zl*f+Line1.Z1;
                    temp1 = div64X64(mul64X32(add64(mul64(Zl,Z01),mul64(Xl,X01)),Zl),mul64(d1,d1));
                    temp2 = div64(mul64(d2,Xl),d1);
                    temp3 = div64(mul64X32(mul64(c,Zl),f),d1);
                    Join_Point.z = temp1.int_l-temp2.int_l-temp3.int_l+Line1.Z1;
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'X',Join_Point.x,'Z',Join_Point.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F);



                }
                if(2==G)//G19
                {
                    int32_t R1,R2,J1,K1,J2,K2;//I1,I2,
                    int a=1;
                    if(Line2.type==3) a=-1;
                    //I1=Line1.I;
                    J1=Line1.J;
                    K1=Line1.K;//I2=Line2.I;
                    J2=Line2.J;K2=Line2.K;

                    //R1=a*sqrt(J1*J1+K1*K1);
                    R1=a*sqr64(add64(mul64(J1,J1),mul64(K1,K1)));
                    //R2=a*sqrt(J2*J2+K2*K2);
                    R2=a*sqr64(add64(mul64(J2,J2),mul64(K2,K2)));

                    Z01=Line1.Z0+K1-Line1.Z1;
                    Y01=Line1.Y0+J1-Line1.Y1;
                    Z02=K2;
                    Y02=J2;
                    //d1=sqrt((Z02-Z01)*(Z02-Z01)+(Y02-Y01)*(Y02-Y01));
                    d1=sqr64(add64(mul64(Z02-Z01,Z02-Z01),mul64(Y02-Y01,Y02-Y01)));
                    //d2=d*(R1-R2)/d1;
                    temp1=div64(mul64(d,R1-R2),d1);
                    d2 = temp1.int_l;

                    //Zl=(-1)*(Y02-Y01)/d1;
                    Zl=(-1)*(Y02-Y01);

                    //Yl=(Z02-Z01)/d1;
                    Yl=Z02-Z01;

                    //b=(Yl*Z01-Zl*Y01-d2);
                    temp1=div64(sub64(sub64(mul64(Yl,Z01),mul64(Zl,Y01)),mul64(d1,d2)),d1);
                    b = temp1.int_l;

                    //c=(Yl*Y01+Zl*Z01);
                    temp1 = div64(add64(mul64(Yl,Y01),mul64(Zl,Z01)),d1);
                    c = temp1.int_l;
                    m=0;
                    if(c>0) m=1; if(c<0)m=-1;

                    //f=sqrt((R1+d)*(R1+d)-b*b);
                    f=sqr64(sub64(mul64(R1+d,R1+d),mul64(b,b)));
                    //Join_Point.z=Zl*(Yl*Y01+Zl*Z01)+d2*Yl-c*Zl*f+Line1.Z1;
                    temp1 = div64X64(mul64X32(add64(mul64(Yl,Y01),mul64(Zl,Z01)),Zl),mul64(d1,d1));
                    temp2 = div64(mul64(d2,Yl),d1);
                    temp3 = div64(mul64X32(mul64(c,Zl),f),d1);
                    Join_Point.z= temp1.int_l+temp2.int_l-temp3.int_l+Line1.Z1;

                    //Join_Point.y=Yl*(Yl*Y01+Zl*Z01)-d2*Zl-c*Yl*f+Line1.Y1;
                    temp1 = div64X64(mul64X32(add64(mul64(Yl,Y01),mul64(Zl,Z01)),Yl),mul64(d1,d1));
                    temp2 = div64(mul64(d2,Zl),d1);
                    temp3 = div64(mul64X32(mul64(c,Yl),f),d1);
                    Join_Point.y= temp1.int_l-temp2.int_l-temp3.int_l+Line1.Y1;
                    sprintf(TempChar,"%s%c%ld%c%ld%c%ld%c%ld%c%ld",G02_3,'Y',Join_Point.y,'Z',Join_Point.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F);


                }
            }
        }//end(Join_Type==0)//缩短形
        if(Join_Type==1)//伸长形
        {
            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    //Join_Point2.y=Line1.Y1+d*Xl1;
                    Join_Point2.y=Line1.Y1+tempx1.int_l;
                    //Join_Point2.x=Line1.X1-d*Yl1;
                    Join_Point2.x=Line1.X1-tempy1.int_l;

                    //Join_Point1.y=(Yl2-Yl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.Y1;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Xl,slope2.Yl),mul64(slope2.Xl,slope1.Yl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Xl1*Yl2-Xl2*Yl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;
                    Point = JointPoint(0,Line1,d,slope1,slope2);
                    Join_Point1.y = Point.y;
                    Join_Point1.x = Point.x;

                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //str_mid.Format(_T("(;%sY%.0fX%.0fI%.0fJ%.0fF%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;)"),
                    //             G02_3,Join_Point2.y*1000,Join_Point2.x*1000,I0*1000,J0*1000,vel*2/15,
                    //             G00_1,Join_Point1.y*1000,Join_Point1.x*1000,
                    //             G00_1,Join_Point.y*1000,Join_Point.x*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point2.x,'Y',Join_Point2.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point2.x=Line1.X1+d*Zl1;
                    //Join_Point2.z=Line1.Z1-d*Xl1;
                    //Join_Point1.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //Join_Point.x=Line1.X1+d*Zl2;
                    //Join_Point.z=Line1.Z1-d*Xl2;

                    //Join_Point2.x=Line1.X1+d*Zl1;
                    Join_Point2.x=Line1.X1+tempz1.int_l;
                    //Join_Point2.z=Line1.Z1-d*Xl1;
                    Join_Point2.z=Line1.Z1-tempx1.int_l;

                    //Join_Point1.x=(Xl2-Xl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.X1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Xl,d),slope1.length),mul64X32(mul64(slope1.Xl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Zl,slope2.Xl),mul64(slope2.Zl,slope1.Xl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.x=temp3.int_l+Line1.X1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Zl1*Xl2-Zl2*Xl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    Point = JointPoint(1,Line1,d,slope1,slope2);
                    Join_Point1.x = Point.x;
                    Join_Point1.z = Point.z;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //str_mid.Format(_T("(;%sX%.0fZ%.0fI%.0fK%.0fF%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;)"),
                    //             G02_3,Join_Point2.x*1000,Join_Point2.z*1000,I0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point1.x*1000,Join_Point1.z*1000,
                    //             G00_1,Join_Point.x*1000,Join_Point.z*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point2.x,'Z',Join_Point2.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point2.z=Line1.Z1+d*Yl1;
                    //Join_Point2.y=Line1.Y1-d*Zl1;
                    //Join_Point1.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //Join_Point.z=Line1.Z1+d*Yl2;
                    //Join_Point.y=Line1.Y1-d*Zl2;

                    //Join_Point2.z=Line1.Z1+d*Yl1;
                    Join_Point2.z=Line1.Z1+tempy1.int_l;
                    //Join_Point2.y=Line1.Y1-d*Zl1;
                    Join_Point2.y=Line1.Y1-tempz1.int_l;

                    //Join_Point1.z=(Zl2-Zl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Z1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Zl,d),slope1.length),mul64X32(mul64(slope1.Zl,d),slope2.length));
                    //temp2 = sub64(mul64(slope1.Yl,slope2.Zl),mul64(slope2.Yl,slope1.Zl));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.z=temp3.int_l+Line1.Z1;
                    //Join_Point1.y=(Yl2-Yl1)*d/(Yl1*Zl2-Yl2*Zl1)+Line1.Y1;
                    //temp1 = sub64(mul64X32(mul64(slope2.Yl,d),slope1.length),mul64X32(mul64(slope1.Yl,d),slope2.length));
                    //temp3 = div64X64(temp1,temp2);
                    //Join_Point1.y=temp3.int_l+Line1.Y1;
                    Point = JointPoint(2,Line1,d,slope1,slope2);
                    Join_Point1.z = Point.z;
                    Join_Point1.y = Point.y;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    //str_mid.Format(_T("(;%sZ%.0fY%.0fJ%.0fK%.0fF%.0f;%sZ%.0fY%.0f;%sZ%.0fY%.0f;)"),
                    //             G02_3,Join_Point2.z*1000,Join_Point2.y*1000,J0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point1.z*1000,Join_Point1.y*1000,
                    //             G00_1,Join_Point.z*1000,Join_Point.y*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'Y',Join_Point2.y,'Z',Join_Point2.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');

                }
            }

        }//end(Join_Type==1)//伸长形
        if(Join_Type==2)//插入形
        {

            if(i==2)//刀补进行
            {
                if(G==0)//G17
                {
                    //Join_Point3.y=Line1.Y1+d*Xl1;
                    //Join_Point3.x=Line1.X1-d*Yl1;
                    //Join_Point2.y=d*Xl1+abs(d)*Yl1+Line1.Y1;
                    //Join_Point2.x=(-1)*d*Yl1+abs(d)*Xl1+Line1.X1;
                    //Join_Point1.y=d*Xl2-abs(d)*Yl2+Line1.Y1;
                    //Join_Point1.x=(-1)*d*Yl2-abs(d)*Xl2+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    //Join_Point.x=Line1.X1-d*Yl2;

                    //Join_Point3.y=Line1.Y1+d*Xl1;
                    Join_Point3.y=Line1.Y1+tempx1.int_l;
                    //Join_Point3.x=Line1.X1-d*Yl1;
                    Join_Point3.x=Line1.X1-tempy1.int_l;

                    //Join_Point2.y=d*Xl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=tempx1.int_l+temp1.int_l+Line1.Y1;
                    //Join_Point2.x=(-1)*d*Yl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=(-1)*(int32_t)tempy1.int_l+(int32_t)temp1.int_l+Line1.X1;

                    //Join_Point1.y=d*Xl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=tempx2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point1.x=(-1)*d*Yl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=(-1)*(int32_t)tempy2.int_l-temp1.int_l+Line1.X1;
                    //Join_Point.y=Line1.Y1+d*Xl2;
                    Join_Point.y=Line1.Y1+tempx2.int_l;
                    //Join_Point.x=Line1.X1-d*Yl2;
                    Join_Point.x=Line1.X1-tempy2.int_l;
                    //str_mid.Format(_T("(;%sY%.0fX%.0fI%.0fJ%.0fF%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;%sY%.0fX%.0f;)"),
                    //             G02_3,Join_Point3.y*1000,Join_Point3.x*1000,I0*1000,J0*1000,vel*2/15,
                    //             G00_1,Join_Point2.y*1000,Join_Point2.x*1000,
                    //             G00_1,Join_Point1.y*1000,Join_Point1.x*1000,
                    //             G00_1,Join_Point.y*1000,Join_Point.x*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point3.x,'Y',Join_Point3.y,'I',I0,'J',J0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point2.x,'Y',Join_Point2.y,';',G00_1,'X',Join_Point1.x,'Y',Join_Point1.y,';',G00_1,'X',Join_Point.x,'Y',Join_Point.y,';');

                }
                if(G==1)//G18
                {
                    //Join_Point3.x=Line1.X1+d*Zl1;
                    //Join_Point3.z=Line1.Z1-d*Xl1;
                    //Join_Point2.x=d*Zl1+abs(d)*Xl1+Line1.X1;
                    //Join_Point2.z=(-1)*d*Xl1+abs(d)*Zl1+Line1.Z1;
                    //Join_Point1.x=d*Zl2-abs(d)*Xl2+Line1.X1;
                    //Join_Point1.z=(-1)*d*Xl2-abs(d)*Zl2+Line1.Z1;
                    //Join_Point.x=Line1.X1+d*Zl2;
                    //Join_Point.z=Line1.Z1-d*Xl2;

                    //Join_Point3.x=Line1.X1+d*Zl1;
                    Join_Point3.x=Line1.X1+tempz1.int_l;
                    //Join_Point3.z=Line1.Z1-d*Xl1;
                    Join_Point3.z=Line1.Z1-tempx1.int_l;
                    //Join_Point2.x=d*Zl1+abs(d)*Xl1+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl1),slope1.length);
                    Join_Point2.x=tempz1.int_l+temp1.int_l+Line1.X1;

                    //Join_Point2.z=(-1)*d*Xl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=(-1)*(int32_t)tempx1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point1.x=d*Zl2-abs(d)*Xl2+Line1.X1;
                    temp1 = div64(mul64(labs(d),Xl2),slope2.length);
                    Join_Point1.x=tempz2.int_l-temp1.int_l+Line1.X1;

                    //Join_Point1.z=(-1)*d*Xl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=(-1)*(int32_t)tempx2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point.x=Line1.X1+d*Zl2;
                    Join_Point.x=Line1.X1+tempz2.int_l;
                    //Join_Point.z=Line1.Z1-d*Xl2;
                    Join_Point.z=Line1.Z1-tempx2.int_l;
                    //str_mid.Format(_T("(;%sX%.0fZ%.0fI%.0fK%.0fF%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;%sX%.0fZ%.0f;)"),
                    //             G02_3,Join_Point3.x*1000,Join_Point3.z*1000,I0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point2.x*1000,Join_Point2.z*1000,
                    //             G00_1,Join_Point1.x*1000,Join_Point1.z*1000,
                    //             G00_1,Join_Point.x*1000,Join_Point.z*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'X',Join_Point3.x,'Z',Join_Point3.z,'I',I0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'X',Join_Point2.x,'Z',Join_Point2.z,';',G00_1,'X',Join_Point1.x,'Z',Join_Point1.z,';',G00_1,'X',Join_Point.x,'Z',Join_Point.z,';');

                }
                if(G==2)//G19
                {
                    //Join_Point3.z=Line1.Z1+d*Yl1;
                    //Join_Point3.y=Line1.Y1-d*Zl1;
                    //Join_Point2.z=d*Yl1+abs(d)*Zl1+Line1.Z1;
                    //Join_Point2.y=(-1)*d*Zl1+abs(d)*Yl1+Line1.Y1;
                    //Join_Point1.z=d*Yl2-abs(d)*Zl2+Line1.Z1;
                    //Join_Point1.y=(-1)*d*Zl2-abs(d)*Yl2+Line1.Y1;
                    //Join_Point.z=Line1.Z1+d*Yl2;
                    //Join_Point.y=Line1.Y1-d*Zl2;

                    //Join_Point3.z=Line1.Z1+d*Yl1;
                    Join_Point3.z=Line1.Z1+tempy1.int_l;
                    //Join_Point3.y=Line1.Y1-d*Zl1;
                    Join_Point3.y=Line1.Y1-tempz1.int_l;
                    //Join_Point2.z=d*Yl1+abs(d)*Zl1+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl1),slope1.length);
                    Join_Point2.z=tempy1.int_l+temp1.int_l+Line1.Z1;

                    //Join_Point2.y=(-1)*d*Zl1+abs(d)*Yl1+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl1),slope1.length);
                    Join_Point2.y=(-1)*(int32_t)tempz1.int_l+temp1.int_l+Line1.Y1;

                    //Join_Point1.z=d*Yl2-abs(d)*Zl2+Line1.Z1;
                    temp1 = div64(mul64(labs(d),Zl2),slope2.length);
                    Join_Point1.z=tempy2.int_l-temp1.int_l+Line1.Z1;

                    //Join_Point1.y=(-1)*d*Zl2-abs(d)*Yl2+Line1.Y1;
                    temp1 = div64(mul64(labs(d),Yl2),slope2.length);
                    Join_Point1.y=(-1)*(int32_t)tempz2.int_l-temp1.int_l+Line1.Y1;

                    //Join_Point.z=Line1.Z1+d*Yl2;
                    Join_Point.z=Line1.Z1+tempy2.int_l;
                    //Join_Point.y=Line1.Y1-d*Zl2;
                    Join_Point.y=Line1.Y1-tempz2.int_l;
                    //str_mid.Format(_T("(;%sZ%.0fY%.0fJ%.0fK%.0fF%.0f;%sZ%.0fY%.0f;%sZ%.0fY%.0f;%sZ%.0fY%.0f;)"),
                    //             G02_3,Join_Point3.z*1000,Join_Point3.y*1000,J0*1000,K0*1000,vel*2/15,
                    //             G00_1,Join_Point2.z*1000,Join_Point2.y*1000,
                    //             G00_1,Join_Point1.z*1000,Join_Point1.y*1000,
                    //             G00_1,Join_Point.z*1000,Join_Point.y*1000);
                    sprintf(TempChar,"%c%s%c%ld%c%ld%c%ld%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c%s%c%ld%c%ld%c",';',G02_3,'Y',Join_Point3.y,'Z',Join_Point3.z,'J',J0,'K',K0,'F',EncodeRegs.CsReg.F,';',G00_1,'Y',Join_Point2.y,'Z',Join_Point2.z,';',G00_1,'Y',Join_Point1.y,'Z',Join_Point1.z,';',G00_1,'Y',Join_Point.y,'Z',Join_Point.z,';');
                }
            }

        }//end(Join_Type==1)//插入形
    }//end(ConnectLines==3)//圆弧接圆弧

    //OutStr = OutStr;
    //strcpy(DbOutStr,OutStr);
    strcpy(OutStr,TempChar);
    return Join_Point;
}
//********************************************
//函数功能：   字符串查找
//输入参数：   char *Buf,char *Buf1,char *Buf2
//输出参数：   void
//创建时间：   2006.03.14
//最后修改时间:2006.03.14
//修改原因：   处理数据需要进行64位数据
//创建人  ：   杨晓龙
//**********************************************
void SearchCstring(char *Buf,char *Buf1,char *Buf2)
{
    int   i=0;
    int   j=0;
    memset(Buf1,0,100);
    memset(Buf2,0,100);

    while(Buf[i] != ';')
    {
           Buf1[i] = Buf[i++];
    }
    i++;
    while(Buf[i]!='\0')
    {
        Buf2[j++]=Buf[i++];
    }
}