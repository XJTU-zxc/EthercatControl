// 直线插补
#include "variabledef.h"
#include <math.h>


extern int32_t d_1,d_2,d_3,d_4,d_5,d_6;//chenlin加速度
struct LineInterST LineInterRegs;
//extern struct Robot_joints joints_max, joints_min;
//extern int32_t finished_length;//chenlin2014.6.21
// ---------------------------------------------------------------------------
// 函数功能：直线插补预计算
// 输入参数：inter_in
// 输出参数：inter_step
// 创建时间：2006.03.21
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_step line_inter_pre(struct inter_in temp)
{
    int32_t temp_x=0,temp_y=0,temp_z=0,temp_a=0,temp_b=0,temp_c=0;
    struct inter_step temp_step;
    struct INT64 temp8;
    temp_step.velo_a=0, temp_step.velo_b=0, temp_step.velo_c=0;
    temp_step.velo_x=0, temp_step.velo_y=0, temp_step.velo_z=0;
    temp8.int_l=0;
    temp8.int_h=0;
    LineInterRegs.lin_remx=0;
    LineInterRegs.lin_remy=0;
    LineInterRegs.lin_remz=0;
    LineInterRegs.lin_rema=0;
    LineInterRegs.lin_remb=0;
    LineInterRegs.lin_remc=0;
    temp_x=labs(temp.x_st - temp.x_end);
    temp_y=labs(temp.y_st - temp.y_end);
    temp_z=labs(temp.z_st - temp.z_end);
    temp_a=labs(temp.a_st - temp.a_end);
    temp_b=labs(temp.b_st - temp.b_end);
    temp_c=labs(temp.c_st - temp.c_end);

    if(temp_x!=0)   // velo*xlength/linelength
    {
        temp8=div64(mul64(temp.velo,temp_x),LineInterRegs.length);      // X轴进给步长
        temp_step.velo_x=temp8.int_l;
        LineInterRegs.lin_remx=temp8.int_h;
    }
    if(temp_y!=0)
    {
        temp8=div64(mul64(temp.velo,temp_y),LineInterRegs.length);      // Y轴进给步长
        temp_step.velo_y=temp8.int_l;
        LineInterRegs.lin_remy=temp8.int_h;
   }
    if(temp_z!=0)
    {
        temp8=div64(mul64(temp.velo,temp_z),LineInterRegs.length);      // Z轴进给步长
        temp_step.velo_z=temp8.int_l;
        LineInterRegs.lin_remz=temp8.int_h;
    }
    if(temp_a!=0)
    {
        temp8=div64(mul64(temp.velo,temp_a),LineInterRegs.length);      // A轴进给步长
        temp_step.velo_a=temp8.int_l;
        LineInterRegs.lin_rema=temp8.int_h;
    }
    if(temp_b!=0)
    {
        temp8=div64(mul64(temp.velo,temp_b),LineInterRegs.length);      // B轴进给步长
        temp_step.velo_b=temp8.int_l;
        LineInterRegs.lin_remb=temp8.int_h;
    }

    if(temp_c!=0)
    {
        temp8=div64(mul64(temp.velo,temp_c),LineInterRegs.length);      // C轴进给步长
        temp_step.velo_c=temp8.int_l;
        LineInterRegs.lin_remc=temp8.int_h;
   }
    return temp_step;
 }
// ---------------------------------------------------------------------------
// 函数功能：直线插补实时坐标运算
// 输入参数：inter_step，inter_in，inter_pos
// 输出参数：inter_pos
// 创建时间：2006.03.21
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
struct inter_pos locomotion_line(struct inter_step velostep, struct inter_in endst, struct inter_pos realpos)// 实时坐标运算
{
//  int onto_flag=0;
    LineInterRegs.add_x += LineInterRegs.lin_remx;
    LineInterRegs.add_y += LineInterRegs.lin_remy;
    LineInterRegs.add_z += LineInterRegs.lin_remz;
    LineInterRegs.add_a += LineInterRegs.lin_rema;
    LineInterRegs.add_b += LineInterRegs.lin_remb;
    LineInterRegs.add_c += LineInterRegs.lin_remc;
    // 到达终点判断
    if( (endst.x_end==realpos.pos_x)&&(endst.y_end==realpos.pos_y)&&(endst.z_end==realpos.pos_z)&&(endst.a_end==realpos.pos_a)&&(endst.b_end==realpos.pos_b)&&(endst.c_end==realpos.pos_c) )
    {}//    onto_flag=1;
    else
    {
        if((LineInterRegs.length-LineInterRegs.velo_add)>endst.velo)            // 总长-步长累加器>一次进给长度
        {
            if(endst.x_end>endst.x_st)                  //X轴处理
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_x = realpos.pos_x + velostep.velo_x + 1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_x = realpos.pos_x+velostep.velo_x;
                }
            }
            else
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_x=realpos.pos_x-velostep.velo_x-1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_x=realpos.pos_x-velostep.velo_x;
                }
            }
            if(endst.y_end>endst.y_st)          //Y轴处理
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_y=realpos.pos_y+velostep.velo_y+1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_y=realpos.pos_y+velostep.velo_y;
                }
            }
            else
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_y=realpos.pos_y-velostep.velo_y-1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_y=realpos.pos_y-velostep.velo_y;
                }
            }
            if(endst.z_end>endst.z_st)      //Z轴处理
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_z=realpos.pos_z+velostep.velo_z+1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_z=realpos.pos_z+velostep.velo_z;
                }
            }
            else
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_z=realpos.pos_z-velostep.velo_z-1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_z=realpos.pos_z-velostep.velo_z;
                }
            }
            if(endst.a_end>endst.a_st)          //A轴处理
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_a=realpos.pos_a+velostep.velo_a+1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_a=realpos.pos_a+velostep.velo_a;
                }
            }
            else
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_a=realpos.pos_a-velostep.velo_a-1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_a=realpos.pos_a-velostep.velo_a;
                }
            }
            if(endst.b_end>endst.b_st)          //B轴处理
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_b=realpos.pos_b+velostep.velo_b+1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_b=realpos.pos_b+velostep.velo_b;
                }
            }
            else
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_b=realpos.pos_b-velostep.velo_b-1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_b=realpos.pos_b-velostep.velo_b;
                }
            }
            if(endst.c_end>endst.c_st)      //C轴处理
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_c=realpos.pos_c+velostep.velo_c+1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_c=realpos.pos_c+velostep.velo_c;
                }
            }
            else
            {
                if(LineInterRegs.add_x>=LineInterRegs.length)
                {
                    realpos.pos_c=realpos.pos_c-velostep.velo_c-1;
                    LineInterRegs.add_x-=LineInterRegs.length;
                }
                else
                {
                    realpos.pos_c=realpos.pos_c-velostep.velo_c;
                }
            }
            LineInterRegs.velo_add=LineInterRegs.velo_add+endst.velo;
        }
        else
        {
            realpos.pos_x=endst.x_end;
            realpos.pos_y=endst.y_end;
            realpos.pos_z=endst.z_end;
            realpos.pos_a=endst.a_end;
            realpos.pos_b=endst.b_end;
            realpos.pos_c=endst.c_end;
        }
    }
    return realpos;
}
// ---------------------------------------------------------------------------
// 函数功能：姿态插补
// 输入参数：无
// 输出参数：无
// 创建时间：2011.03.21
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
//void PoseInter()
//{
//   struct INT64 temp_alfa,temp_gama,temp_beta;
//   struct INT64 numer_alfa,numer_gama,numer_beta;
//   struct INT64 denomin_alfa,denomin_gama,denomin_beta;
//   struct INT64 rem_alfa,rem_gama,rem_beta;
//
///*   long long temp_alfa , temp_gama , temp_beta;
//   long long numer_alfa,numer_gama,numer_beta;
//   long long denomin_alfa,denomin_gama,denomin_beta;
//*/
//   /*temp_alfa = (long long)length_add * (long long)((RobotRegs.robot_pose_inter.Alfa_end - RobotRegs.robot_pose_inter.Alfa_st));
//   temp_gama = (long long)length_add * (long long)((RobotRegs.robot_pose_inter.Gama_end - RobotRegs.robot_pose_inter.Gama_st));
//   temp_beta = (long long)length_add * (long long)((RobotRegs.robot_pose_inter.Beta_end - RobotRegs.robot_pose_inter.Beta_st));
//*/
//
//   temp_alfa = mul64(LineInterRegs.velo_add , (RobotRegs.robot_pose_inter.Alfa_end - RobotRegs.robot_pose_inter.Alfa_st) );
//   temp_gama = mul64(LineInterRegs.velo_add , (RobotRegs.robot_pose_inter.Gama_end - RobotRegs.robot_pose_inter.Gama_st) );
//   temp_beta = mul64(LineInterRegs.velo_add , (RobotRegs.robot_pose_inter.Beta_end - RobotRegs.robot_pose_inter.Beta_st) );
//   //姿态插补结束
//   if(LineInterRegs.finished_inter_pose == 1)
//   {
//         RobotRegs.robot_real_pose.Alfa = RobotRegs.robot_pose_inter.Alfa_end;
//         RobotRegs.robot_real_pose.Gama = RobotRegs.robot_pose_inter.Gama_end;
//         RobotRegs.robot_real_pose.Beta = RobotRegs.robot_pose_inter.Beta_end;
//         LineInterRegs.finished_inter_pose = 0;
//         RobotRegs.robot_pose_inter.pose_change_flag = 0;
//   }
//    else
//   {
//      if( RobotRegs.rem_pose.Alfa > 0 || RobotRegs.rem_pose.Alfa == 0)
//      {   rem_alfa.int_l = RobotRegs.rem_pose.Alfa;
//          rem_alfa.int_h = 0;
//      }else{
//          rem_alfa.int_l = RobotRegs.rem_pose.Alfa;
//          rem_alfa.int_h = -1;
//          }
//
//      if( RobotRegs.rem_pose.Gama > 0 || RobotRegs.rem_pose.Gama == 0)
//      {   rem_gama.int_l = RobotRegs.rem_pose.Gama;
//          rem_gama.int_h = 0;
//      }else{
//          rem_gama.int_l = RobotRegs.rem_pose.Gama;
//          rem_gama.int_h = -1;
//          }
//
//      if( RobotRegs.rem_pose.Beta > 0 || RobotRegs.rem_pose.Beta == 0)
//      {   rem_beta.int_l = RobotRegs.rem_pose.Beta;
//          rem_beta.int_h = 0;
//      }else{
//          rem_beta.int_l = RobotRegs.rem_pose.Beta;
//          rem_beta.int_h = -1;
//          }
//
//     numer_alfa =  add64(temp_alfa , rem_alfa) ;
//     numer_gama =  add64(temp_gama , rem_gama) ;
//     numer_beta =  add64(temp_beta , rem_beta) ;
//
//     denomin_alfa =  div64(numer_alfa , LineInterRegs.length );
//     denomin_gama =  div64(numer_gama , LineInterRegs.length );
//     denomin_beta =  div64(numer_beta , LineInterRegs.length );
//
//     RobotRegs.robot_real_pose.Alfa = RobotRegs.robot_pose_inter.Alfa_st + denomin_alfa.int_l ;
//     RobotRegs.robot_real_pose.Gama = RobotRegs.robot_pose_inter.Gama_st + denomin_gama.int_l ;
//     RobotRegs.robot_real_pose.Beta = RobotRegs.robot_pose_inter.Beta_st + denomin_beta.int_l ;
//
//     RobotRegs.rem_pose.Alfa = denomin_alfa.int_h ;
//     RobotRegs.rem_pose.Gama = denomin_gama.int_h ;
//     RobotRegs.rem_pose.Beta = denomin_beta.int_h ;
//   }
//
//
///*
//   else
//   {
//     //余量处理
//     numer_alfa =  temp_alfa + RobotRegs.rem_pose..Alfa ;
//     numer_gama =  temp_gama + RobotRegs.rem_pose..Gama ;
//     numer_beta =  temp_beta + RobotRegs.rem_pose..Beta ;
//
//     denomin_alfa =  numer_alfa / route_length ;
//     denomin_gama =  numer_gama / route_length ;
//     denomin_beta =  numer_beta / route_length ;
//
//     RobotRegs.robot_real_pose.Alfa = RobotRegs.robot_pose_inter.Alfa_st + denomin_alfa ;
//     RobotRegs.robot_real_pose.Gama = RobotRegs.robot_pose_inter.Gama_st + denomin_gama ;
//     RobotRegs.robot_real_pose.Beta = RobotRegs.robot_pose_inter.Beta_st + denomin_beta ;
//     //计算余量
//     RobotRegs.rem_pose..Alfa = numer_alfa -  denomin_alfa * route_length;
//     RobotRegs.rem_pose..Gama = numer_gama -  denomin_gama * route_length;
//     RobotRegs.rem_pose..Beta = numer_beta -  denomin_beta * route_length;
//
//
//   }*/
//
//  // -------------------------------------------------------------------------
//
//  /* temp_alfa = mul64(LineInterRegs.velo_add , (RobotRegs.robot_pose_inter.Alfa_end - RobotRegs.robot_pose_inter.Alfa_st) );
//   temp_gama = mul64(LineInterRegs.velo_add , (RobotRegs.robot_pose_inter.Gama_end - RobotRegs.robot_pose_inter.Gama_st) );
//   temp_beta = mul64(LineInterRegs.velo_add , (RobotRegs.robot_pose_inter.Beta_end - RobotRegs.robot_pose_inter.Beta_st) );
//
//   if(LineInterRegs.finished_inter_pose == 1)
//   {
//     RobotRegs.robot_real_pose.Alfa = RobotRegs.robot_pose_inter.Alfa_end;
//     RobotRegs.robot_real_pose.Gama = RobotRegs.robot_pose_inter.Gama_end;
//     RobotRegs.robot_real_pose.Beta = RobotRegs.robot_pose_inter.Beta_end;
//     LineInterRegs.finished_inter_pose = 0;
//   }
//   else
//   {
//      if( RobotRegs.rem_pose..Alfa > 0 || RobotRegs.rem_pose..Alfa == 0)
//      {   rem_alfa.int_l = RobotRegs.rem_pose..Alfa;
//          rem_alfa.int_h = 0;
//      }else{
//          rem_alfa.int_l = RobotRegs.rem_pose..Alfa;
//          rem_alfa.int_h = -1;
//          }
//
//      if( RobotRegs.rem_pose..Gama > 0 || RobotRegs.rem_pose..Gama == 0)
//      {   rem_gama.int_l = RobotRegs.rem_pose..Gama;
//          rem_gama.int_h = 0;
//      }else{
//          rem_gama.int_l = RobotRegs.rem_pose..Gama;
//          rem_gama.int_h = -1;
//          }
//
//      if( RobotRegs.rem_pose..Beta > 0 || RobotRegs.rem_pose..Beta == 0)
//      {   rem_beta.int_l = RobotRegs.rem_pose..Beta;
//          rem_beta.int_h = 0;
//      }else{
//          rem_beta.int_l = RobotRegs.rem_pose..Beta;
//          rem_beta.int_h = -1;
//          }
//
//     numer_alfa =  add64(temp_alfa , rem_alfa) ;
//     numer_gama =  add64(temp_gama , rem_gama) ;
//     numer_beta =  add64(temp_beta , rem_beta) ;
//
//     denomin_alfa =  div64(numer_alfa , LineInterRegs.length );
//     denomin_gama =  div64(numer_gama , LineInterRegs.length );
//     denomin_beta =  div64(numer_beta , LineInterRegs.length );
//
//     RobotRegs.robot_real_pose.Alfa = RobotRegs.robot_pose_inter.Alfa_st + denomin_alfa.int_l ;
//     RobotRegs.robot_real_pose.Gama = RobotRegs.robot_pose_inter.Gama_st + denomin_gama.int_l ;
//     RobotRegs.robot_real_pose.Beta = RobotRegs.robot_pose_inter.Beta_st + denomin_beta.int_l ;
//
//     RobotRegs.rem_pose..Alfa = denomin_alfa.int_h ;
//     RobotRegs.rem_pose..Gama = denomin_gama.int_h ;
//     RobotRegs.rem_pose..Beta = denomin_beta.int_h ;
//   }*/
//
//}



// ---------------------------------------------------------------------------
// 函数功能：直线插补
// 输入参数：无
// 输出参数：无
// 创建时间：2006.03.21
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
double test1=0,test2=0,test3=0,test4=0;


// ---------------------------------------------------------------------------
// 函数功能：直线插补//其实是关节插补
// 输入参数：无
// 输出参数：无
// 创建时间：2010.11.21
// 最后修改日期：
// 修改原因：
// ---------------------------------------------------------------------------
int32_t joints_delta1=0,joints_delta2=0,joints_delta3=0,joints_delta4=0,joints_delta5=0,joints_delta6=0;
void JointsChabu(void)
{
    if(RunstatusRegs.run_cnt==1)
   {
        speed_pre();
 //     RunstatusRegs.st_end.x_st=joints.p1;            // 直线测试
//      RunstatusRegs.st_end.y_st=joints.p2;
//      RunstatusRegs.st_end.z_st=joints.p3;
//      RunstatusRegs.st_end.a_st=joints.p4;
//      RunstatusRegs.st_end.b_st=joints.p5;
//      RunstatusRegs.st_end.c_st=joints.p6;
//        RunstatusRegs.st_end.x_end=EncodeRegs.AsReg.D;
//        RunstatusRegs.st_end.y_end=EncodeRegs.AsReg.E;
//        RunstatusRegs.st_end.z_end=EncodeRegs.AsReg.N;
//        RunstatusRegs.st_end.a_end=EncodeRegs.AsReg.O;
//        RunstatusRegs.st_end.b_end=EncodeRegs.AsReg.Q;
//        RunstatusRegs.st_end.c_end=EncodeRegs.AsReg.V;
        RunstatusRegs.st_end.x_st=PostionRegs.real_pos.pos_x;            // 直线测试
        RunstatusRegs.st_end.y_st=PostionRegs.real_pos.pos_y;
        RunstatusRegs.st_end.z_st=PostionRegs.real_pos.pos_z;
        RunstatusRegs.st_end.a_st=PostionRegs.real_pos.pos_a;
        RunstatusRegs.st_end.b_st=PostionRegs.real_pos.pos_b;
        RunstatusRegs.st_end.c_st=PostionRegs.real_pos.pos_c;
        RunstatusRegs.st_end.x_end=EncodeRegs.AsReg.X;
        RunstatusRegs.st_end.y_end=EncodeRegs.AsReg.Y;
        RunstatusRegs.st_end.z_end=EncodeRegs.AsReg.Z;
        RunstatusRegs.st_end.a_end=EncodeRegs.AsReg.A;
        RunstatusRegs.st_end.b_end=EncodeRegs.AsReg.B;
        RunstatusRegs.st_end.c_end=EncodeRegs.AsReg.C;

        LineInterRegs.add_x=0;
        LineInterRegs.add_x=0;
        LineInterRegs.add_x=0;
        LineInterRegs.add_x=0;
        LineInterRegs.add_x=0;
        LineInterRegs.add_x=0;
        LineInterRegs.velo_add=0;
        LineInterRegs.lin_remx=0;
        LineInterRegs.lin_remy=0;
        LineInterRegs.lin_remz=0;
        LineInterRegs.lin_rema=0;
        LineInterRegs.lin_remb=0;
        LineInterRegs.lin_remc=0;
        LineInterRegs.finished_length = 0;//chenlin2014.6.21ceshi
//      jointsBK.p1 = joints.p1;
//      jointsBK.p2 = joints.p2;
//      jointsBK.p3 = joints.p3;
//      jointsBK.p4 = joints.p4;
//      jointsBK.p5 = joints.p5;
//      jointsBK.p6 = joints.p6;
        RunstatusRegs.run_cnt=0;
    }
    // 前加减速控制
    speed_control();
    LineInterRegs.finished_length +=  SpeedRegs.sect_in_feed_BK;
    LineInterRegs.length = SpeedRegs.sect_in.length;    // 进给总长度
    RunstatusRegs.st_end.velo=SpeedRegs.sect_in_feed_BK;    // 苏州加减速后的速度
    LineInterRegs.velo_step=line_inter_pre(RunstatusRegs.st_end);
 //   PostionRegs.real_pos4.pos_x = joints.p1;
//  PostionRegs.real_pos4.pos_y = joints.p2;
//  PostionRegs.real_pos4.pos_z = joints.p3;
//  PostionRegs.real_pos4.pos_a = joints.p4;
//  PostionRegs.real_pos4.pos_b = joints.p5;
//  PostionRegs.real_pos4.pos_c = joints.p6;
    PostionRegs.real_pos=locomotion_line(LineInterRegs.velo_step,RunstatusRegs.st_end,PostionRegs.real_pos);
//    joint = joints;//chenlinceshi 2013.11.28

      if((RunstatusRegs.st_end.x_end==PostionRegs.real_pos.pos_x)&&(RunstatusRegs.st_end.y_end==PostionRegs.real_pos.pos_y)&&
      (RunstatusRegs.st_end.z_end==PostionRegs.real_pos.pos_z)&&(RunstatusRegs.st_end.a_end==PostionRegs.real_pos.pos_a)&&
      (RunstatusRegs.st_end.b_end==PostionRegs.real_pos.pos_b)&&(RunstatusRegs.st_end.c_end==PostionRegs.real_pos.pos_c))
    {
         RunstatusRegs.RunStatusFlag.bit.CBAFlag=0;
    }  //glh

//    joints.p1 = PostionRegs.real_pos4.pos_x;
//  joints.p2 = PostionRegs.real_pos4.pos_y;
//  joints.p3 = PostionRegs.real_pos4.pos_z;
//  joints.p4 = PostionRegs.real_pos4.pos_a;
//  joints.p5 = PostionRegs.real_pos4.pos_b;
//  joints.p6 = PostionRegs.real_pos4.pos_c;


//   d_1=joints.p1-joint.p1;
 //    d_2=joints.p2-joint.p2;
//   d_3=joints.p3-joint.p3;
//   d_4=joints.p4-joint.p4;
//   d_5=joints.p5-joint.p5;
//   d_6=joints.p6-joint.p6;//chenlinceshi2013.11.28

//  T6 = cal_t6(joints);//正运动学
//  joints_test = robot_rev(T6);
//   T6ToEuler(T6);//T6阵到欧拉方程

/*  if(abs(joints.p1-joints_test.p1) > joints_delta1)
       joints_delta1 = abs(joints.p1-joints_test.p1);

    if(abs(joints.p2-joints_test.p2) > joints_delta2)
       joints_delta2 = abs(joints.p2-joints_test.p2);

    if(abs(joints.p3-joints_test.p3) > joints_delta3)
       joints_delta3 = abs(joints.p3-joints_test.p3);

    if(abs(joints.p4-joints_test.p4) > joints_delta4)
       joints_delta4 = abs(joints.p4-joints_test.p4);

    if(abs(joints.p5-joints_test.p5) > joints_delta5)
       joints_delta5 = abs(joints.p5-joints_test.p5);

    if(abs(joints.p6-joints_test.p6) > joints_delta6)
       joints_delta6 = abs(joints.p6-joints_test.p6);*/

//    PostionRegs.real_pos.pos_x = T6.px;
//  PostionRegs.real_pos.pos_y = T6.py;
//  PostionRegs.real_pos.pos_z = T6.pz;

    PostionRegs.real_pos.pos_a = EncodeRegs.AsReg.A;//chenlin2014.3.8
    PostionRegs.real_pos.pos_b = EncodeRegs.AsReg.B;
    PostionRegs.real_pos.pos_c = EncodeRegs.AsReg.C;


//  CBAFlag=CBAFlag&0xFE;
}


