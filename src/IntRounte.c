
#include "variabledef.h"
#include "stdlib.h"
#include <math.h>

// #include "params.h"

// ---------------------------------------------------------------------------
// 函数功能：   电子齿轮
// 输入参数：   无
// 输出参数：   无
// 创建时间：   2006.06.12
// 最后修改时间:2006.06.12
// 创建人  ：   付科
// ---------------------------------------------------------------------------
void EGear(void)
{
	struct inter_pos out_tmp;
	// 当前输出脉冲乘以倍频比,然后加上上次计算的余数// M处理时间
	out_tmp.pos_x = PostionRegs.out_imp.pos_x * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[0] + PostionRegs.out_add.pos_x; // (*(uint16_t *)0x80058
	out_tmp.pos_y = PostionRegs.out_imp.pos_y * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[1] + PostionRegs.out_add.pos_y; // 指令倍乘比
	out_tmp.pos_z = PostionRegs.out_imp.pos_z * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[2] + PostionRegs.out_add.pos_z; // 指令倍乘比
	out_tmp.pos_a = PostionRegs.out_imp.pos_a * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[3] + PostionRegs.out_add.pos_a; // 指令倍乘比
	out_tmp.pos_b = PostionRegs.out_imp.pos_b * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[4] + PostionRegs.out_add.pos_b; // 指令倍乘比
	out_tmp.pos_c = PostionRegs.out_imp.pos_c * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[5] + PostionRegs.out_add.pos_c; // 指令倍乘比
	out_tmp.pos_w = PostionRegs.out_imp.pos_w * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[6] + PostionRegs.out_add.pos_w; // 指令倍乘比
	out_tmp.pos_u = PostionRegs.out_imp.pos_u * ParamterUnRegs.ParamterRegs.AxisparmRegs.MulRatios[7] + PostionRegs.out_add.pos_u; // 指令倍乘比
	// 除以分频比
	PostionRegs.out_imp.pos_x = out_tmp.pos_x / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[0]; // // 指令分频(*(uint16_t *)0x8005E)
	PostionRegs.out_imp.pos_y = out_tmp.pos_y / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[1]; // 指令分频(*(uint16_t *)0x8005F)
	PostionRegs.out_imp.pos_z = out_tmp.pos_z / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[2]; // 指令分频(*(uint16_t *)0x80060)
	PostionRegs.out_imp.pos_a = out_tmp.pos_a / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[3]; // 指令分频(*(uint16_t *)0x80061);
	PostionRegs.out_imp.pos_b = out_tmp.pos_b / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[4]; // 指令分频(*(uint16_t *)0x80062);
	PostionRegs.out_imp.pos_c = out_tmp.pos_c / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[5]; // 指令分频(*(uint16_t *)0x80063);
	PostionRegs.out_imp.pos_w = out_tmp.pos_w / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[6]; // 指令分频(*(uint16_t *)0x80062);
	PostionRegs.out_imp.pos_u = out_tmp.pos_u / ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[7]; // 指令分频(*(uint16_t *)0x80063);
	// 余数保存
	PostionRegs.out_add.pos_x = out_tmp.pos_x - PostionRegs.out_imp.pos_x * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[0]; // // 指令分频(*(uint16_t *)0x8005E)
	PostionRegs.out_add.pos_y = out_tmp.pos_y - PostionRegs.out_imp.pos_y * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[1]; // 指令分频(*(uint16_t *)0x8005F)
	PostionRegs.out_add.pos_z = out_tmp.pos_z - PostionRegs.out_imp.pos_z * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[2]; // 指令分频(*(uint16_t *)0x80060)
	PostionRegs.out_add.pos_a = out_tmp.pos_a - PostionRegs.out_imp.pos_a * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[3]; // 指令分频(*(uint16_t *)0x80061);
	PostionRegs.out_add.pos_b = out_tmp.pos_b - PostionRegs.out_imp.pos_b * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[4]; // 指令分频(*(uint16_t *)0x80062);
	PostionRegs.out_add.pos_c = out_tmp.pos_c - PostionRegs.out_imp.pos_c * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[5]; // 指令分频(*(uint16_t *)0x80063);
	PostionRegs.out_add.pos_w = out_tmp.pos_w - PostionRegs.out_imp.pos_w * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[6]; // 指令分频(*(uint16_t *)0x80062);
	PostionRegs.out_add.pos_u = out_tmp.pos_u - PostionRegs.out_imp.pos_u * ParamterUnRegs.ParamterRegs.AxisparmRegs.DivideRatios[7]; // 指令分频(*(uint16_t *)0x80063);
}
// ---------------------------------------------------------------------------
// 函数功能：   G52-G59代码的处理
// 输入参数：   无..
// 输出参数：   无
// 创建时间：   2006.03.24
// 最后修改时间:2006.02.24
// 修改原因：
// 创建人  ：   杨晓龙
// ---------------------------------------------------------------------------
void CorpConv(void)
{
	// G52-G59代码的处理
	switch (EncodeRegs.AsReg.Co)
	{
	case 52: // G52
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	case 53: // G53
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	case 54: // G54
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	case 55: // G55
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	case 56:
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	case 57: // G57
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	case 58: // G58
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	case 59: // G59
		PostionRegs.AbsPos.pos_x = PostionRegs.real_pos.pos_x - PostionRegs.DuiDao.pos_x;
		PostionRegs.AbsPos.pos_y = PostionRegs.real_pos.pos_y - PostionRegs.DuiDao.pos_y;
		PostionRegs.AbsPos.pos_z = PostionRegs.real_pos.pos_z - PostionRegs.DuiDao.pos_z;
		PostionRegs.AbsPos.pos_a = PostionRegs.real_pos.pos_a - PostionRegs.DuiDao.pos_a;
		PostionRegs.AbsPos.pos_b = PostionRegs.real_pos.pos_b - PostionRegs.DuiDao.pos_b;
		PostionRegs.AbsPos.pos_c = PostionRegs.real_pos.pos_c - PostionRegs.DuiDao.pos_c;
		PostionRegs.AbsPos.pos_w = PostionRegs.real_pos.pos_w - PostionRegs.DuiDao.pos_w;
		PostionRegs.AbsPos.pos_u = PostionRegs.real_pos.pos_u - PostionRegs.DuiDao.pos_u;
		break;
	default:
		break;
	}
}
// ---------------------------------------------------------------------------
// 函数功能：   S代码
// 输入参数：   S值
// 输出参数：   PWM占空比
// 创建时间：   2006.09.10
// 最后修改时间:2006.09.10
// 修改原因：
// 创建人  ：   付科
// ---------------------------------------------------------------------------
int SCode(int S)
{
	int32_t tmp;
	int k; // 主轴倍率
	k = ((uint16_t)(0xA000 & 0xE000) >> 13) * 10 + 50;
	tmp = S * k;
	S = tmp / 100;
	// 按挡位划分后的占空比
	if (abs(S) <= ParamterUnRegs.ParamterRegs.MAxisparmRegs.S1RotSpeed) // (*(uint16_t *)0x8006A)  // 1档转速
	{
		tmp = (int32_t)S * MainAxisRegs.PSANGN * 5 / ParamterUnRegs.ParamterRegs.MAxisparmRegs.S1RotSpeed + 5000;
		//		flag = 1;
	}
	else if (abs(S) <= ParamterUnRegs.ParamterRegs.MAxisparmRegs.S2RotSpeed) // 2档转速
	{
		tmp = (int32_t)S * MainAxisRegs.PSANGN * 5 / ParamterUnRegs.ParamterRegs.MAxisparmRegs.S2RotSpeed + 5000;
		//		flag = 2;
	}
	else if (abs(S) <= ParamterUnRegs.ParamterRegs.MAxisparmRegs.S3RotSpeed) // 3档转速
	{
		tmp = (int32_t)S * MainAxisRegs.PSANGN * 5 / ParamterUnRegs.ParamterRegs.MAxisparmRegs.S3RotSpeed + 5000;
		//		flag = 3;
	}
	else // 4档转速
	{
		tmp = (int32_t)S * MainAxisRegs.PSANGN * 5 / ParamterUnRegs.ParamterRegs.MAxisparmRegs.S4RotSpeed + 5000;
		//		flag = 4;
	}

	if (tmp >= 10000)
		tmp = 10000;
	if (tmp <= 1)
		tmp = 1;

	return tmp;
}
// ---------------------------------------------------------------------------
// 函数功能：   实际进给脉冲输出
// 输入参数：   电机旋转方向参数dir,输出脉冲数pls,轴地址*addr
// 输出参数：   无
// 创建时间：   2006.06.10
// 最后修改时间:2006.06.10
// 修改原因：
// 创建人  ：   付科
// ---------------------------------------------------------------------------
/*
void Impls_Out(int bit, int32_t pls)
{
	uint16_t temp;
	uint16_t DirBit = 1<<bit;
	temp = (uint16_t)labs(pls);
	if( temp > 4095)
		temp = 4095;

	if (ParamterUnRegs.ParamterRegs.AxisparmRegs.MotorRotDir & DirBit)   // 电机方向
	{
		if (pls>0)
			temp = temp | 0x1000;				// 12位为1
		else if (pls<0)
			temp = temp & (~0x1000);				// 12位为1
		else
			temp = RunstatusRegs.OutFPGAPulse[bit] & 0x1000;

	}
	else
	{
		if (pls<0)
			temp = temp | 0x1000;				// 12位为1
		else if (pls>0)
			temp = temp & (~0x1000);				// 12位为1
		else
			temp = RunstatusRegs.OutFPGAPulse[bit] & 0x1000;
	}
	RunstatusRegs.OutFPGAPulse[bit] = temp;
}
*/
void MainAxisControl()
{
	// 主轴S代码处理
	int temp = 0;
	if (!RunstatusRegs.Alarm.limit.all)
	{
		if ((RunstatusRegs.RunStatusFlag.bit.Type == 3) || (RunstatusRegs.RunStatusFlag.bit.Type == 2) || (RunstatusRegs.RunStatusFlag.bit.Type == 7))
		{
			// 			temp = InputB & 0x0E00;
			if ((MainAxisRegs.S_button == 0x0200 && temp == 0x0400) || (MainAxisRegs.S_button == 0x0400 && temp == 0x0200) || (MainAxisRegs.S_button == 0x0800 && temp != 0x0800)) // 主轴换向判断
				temp = 1;
			else
			{
				temp = 0;
			}
		}
		// 主轴速度变化
		if ((MainAxisRegs.S_speed != abs(EncodeRegs.AsReg.S) || temp))
		{
			MainAxisRegs.S_time = ParamterUnRegs.ParamterRegs.m_commonparmRegs.STTimes * 5; // S代码处理时间
			MainAxisRegs.S_speed = abs(EncodeRegs.AsReg.S);
			temp = 0;
		}
		//		MainAxisRegs.PWM_out = SCode(MainAxisRegs.S_speed);
		// 实际占空比
		if (MainAxisRegs.PWM_out < 5500) // 主轴死区电压1V
			MainAxisRegs.PWM_out = 5000;
		//		DPWMOUT(MainAxisRegs.PWM_out);
		if (MainAxisRegs.S_time > 0) // 等待S代码完成
		{
			MainAxisRegs.S_time--;
			// 各轴输出为0
			//			Axis_Stop();
			if ((MainAxisRegs.S_time == 0) && (EncodeRegs.AsReg.Type == 100)) // S代码单独执行时的情况
				RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
			return;
		}
	}
}
// int *dir;						// 方向调整完成标志
uint8_t lap_X, lap_Y, lap_Z, lap_A, lap_B, lap_C;		 // 各轴一转信号
uint16_t slow_X, slow_Y, slow_Z, slow_A, slow_B, slow_C; // 各轴减速信号
// ---------------------------------------------------------------------------
// 函数名称:	 ReturnR_axis
// 函数功能：   单轴回参子程序
// 输入参数：   轴选择(axis),轴回参完成标志(*finished),轴实时位置(*pos)
// 输出参数：   当前速度
// 全局变量:	 栅格标志,减速信号,一转信号,上次减速信号,低速状态标志,方向调整完成标志
//					 轴参考点位置,偏置距离,轴脉冲输出地址,快速进给速度,慢速进给速度
// 创建时间：   2006.06.15
// 创建人  ：   付科
// ---------------------------------------------------------------------------
int32_t ReturnR_Axis(int axis, int *finished, int32_t *pos, int32_t speed_rt)
{
	int *slow_flag_bak;		  // 上次减速信号
	int *low_speed_flag;	  // 低速状态标志
	int *dir;				  // 方向调整完成标志
	int flag1, flag2;		  // flag1,减速信号;flag2,一转信号
	int gd_axis;			  // 单轴有无栅格标志
	int32_t speed;			  // speed,快速进给速度
	int32_t pos1;			  // pos1,参考点坐标;pos2,实时坐标
	int32_t *ret_offset_temp; // 偏置距离临时值
	int spdpara;			  // 加减速参数
	switch (axis)
	{
	case 1:
		flag1 = slow_X ^ (ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevel & 0x0001); // 回参减速信号电平 0：高电平	1：低电平
		flag2 = lap_X;
		speed = RPDAccDecRegs.UniVelo_X;
		pos1 = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[0]; // 回参坐标
		dir = &MainAxisRegs.dir_comp.x;
		low_speed_flag = &MainAxisRegs.low_speed.x;
		ret_offset_temp = &PostionRegs.offset_temp.pos_x;
		slow_flag_bak = &MainAxisRegs.slow_bak.x;
		gd_axis = ParamterUnRegs.ParamterRegs.AxisparmRegs.GridSignal & 0x0001; // 栅格信号(*(uint16_t *)0x8001C)
		spdpara = RPDAccDecRegs.spd_down_par1;
		break;
	case 2:
		flag1 = slow_Y ^ ((ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevel & 0x0002) > 1);
		flag2 = lap_Y;
		speed = RPDAccDecRegs.UniVelo_Y;
		pos1 = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[1]; // 回参坐标
		dir = &MainAxisRegs.dir_comp.y;
		low_speed_flag = &MainAxisRegs.low_speed.y;
		ret_offset_temp = &PostionRegs.offset_temp.pos_y;
		slow_flag_bak = &MainAxisRegs.slow_bak.y;
		gd_axis = ParamterUnRegs.ParamterRegs.AxisparmRegs.GridSignal & 0x0002;
		spdpara = RPDAccDecRegs.spd_down_par2;
		break;
	case 3:
		flag1 = slow_Z ^ ((ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevel & 0x0004) > 2);
		flag2 = lap_Z;
		speed = RPDAccDecRegs.UniVelo_Z;
		pos1 = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[2]; // 回参坐标
		dir = &MainAxisRegs.dir_comp.z;
		low_speed_flag = &MainAxisRegs.low_speed.z;
		ret_offset_temp = &PostionRegs.offset_temp.pos_z;
		slow_flag_bak = &MainAxisRegs.slow_bak.z;
		gd_axis = ParamterUnRegs.ParamterRegs.AxisparmRegs.GridSignal & 0x0004;
		spdpara = RPDAccDecRegs.spd_down_par3;
		break;
	case 4:
		flag1 = slow_A ^ ((ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevel & 0x0008) > 3);
		flag2 = lap_A;
		speed = RPDAccDecRegs.UniVelo_A;
		pos1 = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[3]; // 回参坐标
		dir = &MainAxisRegs.dir_comp.a;
		low_speed_flag = &MainAxisRegs.low_speed.a;
		ret_offset_temp = &PostionRegs.offset_temp.pos_a;
		slow_flag_bak = &MainAxisRegs.slow_bak.a;
		gd_axis = ParamterUnRegs.ParamterRegs.AxisparmRegs.GridSignal & 0x0008;
		spdpara = RPDAccDecRegs.spd_down_par4;
		break;
	case 5:
		flag1 = slow_B ^ ((ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevel & 0x0010) > 4);
		flag2 = lap_B;
		speed = RPDAccDecRegs.UniVelo_B;
		pos1 = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[4]; // 回参坐标
		dir = &MainAxisRegs.dir_comp.b;
		low_speed_flag = &MainAxisRegs.low_speed.b;
		ret_offset_temp = &PostionRegs.offset_temp.pos_b;
		slow_flag_bak = &MainAxisRegs.slow_bak.b;
		gd_axis = ParamterUnRegs.ParamterRegs.AxisparmRegs.GridSignal & 0x0010;
		spdpara = RPDAccDecRegs.spd_down_par5;
		break;
	case 6:
		flag1 = slow_C ^ ((ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefLevel & 0x0020) > 5);
		flag2 = lap_C;
		speed = RPDAccDecRegs.UniVelo_C;
		pos1 = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[5]; // 回参坐标
		dir = &MainAxisRegs.dir_comp.c;
		low_speed_flag = &MainAxisRegs.low_speed.c;
		ret_offset_temp = &PostionRegs.offset_temp.pos_c;
		slow_flag_bak = &MainAxisRegs.slow_bak.c;
		gd_axis = ParamterUnRegs.ParamterRegs.AxisparmRegs.GridSignal & 0x0020;
		spdpara = (int32_t)RPDAccDecRegs.spd_down_par6;
		break;
	default:
		//		EvaRegs.EVAIMRA.bit.T1PINT = 1;	// T1周期中断使能
		//		EvaRegs.EVAIFRA.all = BIT7;		// 擦除EVA中断标志寄存器
		//		PieCtrl.PIEACK.all = PIEACK_GROUP2;	// 扩展中断使能
		return speed_rt;
	}
	if (*finished == 1)
	{
		*dir = 0;
		//		EvaRegs.EVAIMRA.bit.T1PINT = 1;			// T1周期中断使能
		//		EvaRegs.EVAIFRA.all = BIT7;				// 擦除EVA中断标志寄存器
		//		PieCtrl.PIEACK.all = PIEACK_GROUP2;		// 扩展中断使能
		return 0;
	}
	if (*dir == 1 || *dir == 2) // 负方向回参方向正确
	{
		if (0 == flag1)
		{
			if (speed_rt < speed)
				speed_rt = speed_rt + 1;
			if (speed_rt >= speed)
				speed_rt = speed;
		}
		if (gd_axis == 0) // 无栅格
		{
			if (0 == flag1) // 无减速信号
			{
				if (0 == *low_speed_flag) // 未在低速状态
				{
					*pos = *pos - speed_rt;
					*slow_flag_bak = flag1;
				}
				else // 低速状态
				{
					if (1 == *slow_flag_bak) // 减速信号负跳变,进给偏置脉冲
					{
						*ret_offset_temp = (int32_t)RunstatusRegs.ret_offset; // 偏置脉冲赋初值
						if (0 == *ret_offset_temp)							  // 偏置脉冲预设为0
						{
							*slow_flag_bak = flag1;
							*low_speed_flag = 0;
							*finished = 1;
						}
						else if (*ret_offset_temp <= RPDAccDecRegs.UniVelo_R) // 偏置脉冲预设值不够一次进给
						{
							*pos = *pos - *ret_offset_temp;
							*ret_offset_temp = 0;
							*slow_flag_bak = flag1;
						}
						else // 开始进给
						{
							*pos = *pos - RPDAccDecRegs.UniVelo_R;
							*ret_offset_temp = *ret_offset_temp - RPDAccDecRegs.UniVelo_R;
							*slow_flag_bak = flag1;
						}
					}
					else if (0 == *slow_flag_bak) // 处于进给偏置脉冲状态状态
					{
						if (0 == *ret_offset_temp)
						{
							*slow_flag_bak = flag1;
							*low_speed_flag = 0;
							*finished = 1;
						}
						else if (*ret_offset_temp <= RPDAccDecRegs.UniVelo_R)
						{
							*pos = *pos - *ret_offset_temp;
							*ret_offset_temp = 0;
							*slow_flag_bak = flag1;
						}
						else
						{
							*pos = *pos - RPDAccDecRegs.UniVelo_R;
							*ret_offset_temp = *ret_offset_temp - RPDAccDecRegs.UniVelo_R;
							*slow_flag_bak = flag1;
						}
					}
				}
			}
			else // 有减速信号
			{
				if (speed_rt > RPDAccDecRegs.UniVelo_R)
					speed_rt = speed_rt - 100;
				if (speed_rt <= RPDAccDecRegs.UniVelo_R)
				{
					speed_rt = RPDAccDecRegs.UniVelo_R;
					*low_speed_flag = 1;
					*slow_flag_bak = flag1;
				}
				*pos = *pos - speed_rt;
			}
		}

		if (gd_axis != 0) // 有栅格
		{
			if (1 == flag1) // 有减速信号
			{
				if (speed_rt > RPDAccDecRegs.UniVelo_R)
					speed_rt = speed_rt - 100;
				//               speed_rt=5;
				if (speed_rt <= RPDAccDecRegs.UniVelo_R)
				{
					speed_rt = RPDAccDecRegs.UniVelo_R;
					*low_speed_flag = 1;
				}
				*pos = *pos - speed_rt;
			}
			else // 无减速信号
			{
				if (0 == *low_speed_flag) // 不在低速状态
					*pos = *pos - speed_rt;
				else // 已在低速状态
				{
					//				DioRegs.LapEn1 = 0x003F;	// 一转信号使能
					if (flag2 == 0)			 // 检测到一转信号
					{						 // 当前轴回参结束
											 //					DioRegs.LapEn1 = 0x0000;// 一转信号不使能
						*low_speed_flag = 0; // 低速标志清零
						*finished = 1;
					}
					else
					{
						*pos = *pos - RPDAccDecRegs.UniVelo_R;
						*low_speed_flag = 1;
					}
				}
			}
		}
	}
	if (*dir == -3) // 减速开关已完成一次置位复位
	{
		if (flag1 == 0) // 减速
		{
			if (speed_rt > 0)
			{
				speed_rt = speed_rt - (0.01) * ((speed * 100) / spdpara);
			}

			if (speed_rt <= 0)
			{
				speed_rt = 0;
				*dir = 2;
			}
		}
		*pos = *pos + speed_rt;
	}

	if (*dir == -2) // 调整中
	{
		*pos = *pos + speed_rt;
		if (flag1 == 1) // 减速开关置位
			*dir = -3;
	}
	if (*dir == -1) // 负位置
	{
		if (0 == flag1)
		{
			if (speed_rt < speed)
				speed_rt = speed_rt + 100;
			if (speed_rt >= speed)
				speed_rt = speed;
		}
		*pos = *pos + speed_rt;
		if (*pos > pos1)
			*dir = -2;
	}
	return speed_rt;
}

/* 回机械零点 */
void RetRefPoint_old(void)
{
	// if(RunstatusRegs.ReturnFlag.bit.x == 1)
	if (1)
	{
		if (0)
			// if ( (ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0001) == 0)		// 0x80091若无机械零点,回参直接结束
			RunstatusRegs.ret_finished_x = 1;
		else // 有机械零点
		{
			RunstatusRegs.ReturnSpeed[0] = ReturnR_Axis(1, &RunstatusRegs.ret_finished_x, &PostionRegs.real_pos.pos_x, RunstatusRegs.ReturnSpeed[0]);
		}
		if (RunstatusRegs.ret_finished_x == 1) // 回参完成
		{
			PostionRegs.real_pos.pos_x = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[0]; // 当前坐标设置为参考点预设坐标
			PostionRegs.real_posbk.pos_x = PostionRegs.real_pos.pos_x;
			RunstatusRegs.ReturnSpeed[0] = 0;
			RunstatusRegs.ReturnFlag.bit.x = 0;
		}
	}
	if (RunstatusRegs.ReturnFlag.bit.y == 1)
	{
		if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0002) == 0)
			RunstatusRegs.ret_finished_y = 1;
		else
		{
			RunstatusRegs.ReturnSpeed[1] = ReturnR_Axis(2, &RunstatusRegs.ret_finished_y, &PostionRegs.real_pos.pos_y, RunstatusRegs.ReturnSpeed[1]);
		}
		if (RunstatusRegs.ret_finished_y == 1)
		{
			PostionRegs.real_pos.pos_y = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[1];
			PostionRegs.real_posbk.pos_y = PostionRegs.real_pos.pos_y;
			RunstatusRegs.ReturnSpeed[1] = 0;
			RunstatusRegs.ReturnFlag.bit.y = 0;
		}
	}
	if (RunstatusRegs.ReturnFlag.bit.z == 1)
	{
		if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0004) == 0)
			RunstatusRegs.ret_finished_z = 1;
		else
		{
			RunstatusRegs.ReturnSpeed[2] = ReturnR_Axis(3, &RunstatusRegs.ret_finished_z, &PostionRegs.real_pos.pos_z, RunstatusRegs.ReturnSpeed[2]);
		}
		if (RunstatusRegs.ret_finished_z == 1)
		{
			PostionRegs.real_pos.pos_z = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[2];
			PostionRegs.real_posbk.pos_z = PostionRegs.real_pos.pos_z;
			RunstatusRegs.ReturnSpeed[2] = 0;
			RunstatusRegs.ReturnFlag.bit.z = 0;
		}
	}
	if (RunstatusRegs.ReturnFlag.bit.a == 1)
	{
		if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0008) == 0)
			RunstatusRegs.ret_finished_a = 1;
		else
		{
			RunstatusRegs.ReturnSpeed[3] = ReturnR_Axis(4, &RunstatusRegs.ret_finished_a, &PostionRegs.real_pos.pos_a, RunstatusRegs.ReturnSpeed[3]);
		}
		if (RunstatusRegs.ret_finished_a == 1)
		{
			PostionRegs.real_pos.pos_a = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[3];
			PostionRegs.real_posbk.pos_a = PostionRegs.real_pos.pos_a;
			RunstatusRegs.ReturnSpeed[3] = 0;
			RunstatusRegs.ReturnFlag.bit.a = 0;
		}
	}
	if (RunstatusRegs.ReturnFlag.bit.b == 1)
	{
		if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0010) == 0)
			RunstatusRegs.ret_finished_b = 1;
		else
		{
			RunstatusRegs.ReturnSpeed[4] = ReturnR_Axis(5, &RunstatusRegs.ret_finished_b, &PostionRegs.real_pos.pos_b, RunstatusRegs.ReturnSpeed[4]);
		}
		if (RunstatusRegs.ret_finished_b == 1)
		{
			PostionRegs.real_pos.pos_b = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[4];
			PostionRegs.real_posbk.pos_b = PostionRegs.real_pos.pos_b;
			RunstatusRegs.ReturnSpeed[4] = 0;
			RunstatusRegs.ReturnFlag.bit.b = 0;
		}
	}
	if (RunstatusRegs.ReturnFlag.bit.c == 1)
	{
		if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0020) == 0)
			RunstatusRegs.ret_finished_c = 1;
		else
		{
			RunstatusRegs.ReturnSpeed[5] = ReturnR_Axis(6, &RunstatusRegs.ret_finished_c, &PostionRegs.real_pos.pos_c, RunstatusRegs.ReturnSpeed[5]);
		}
		if (RunstatusRegs.ret_finished_c == 1)
		{
			PostionRegs.real_pos.pos_c = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[5];
			PostionRegs.real_posbk.pos_c = PostionRegs.real_pos.pos_c;
			RunstatusRegs.ReturnSpeed[5] = 0;
			RunstatusRegs.ReturnFlag.bit.c = 0;
		}
	}
	// G28指令回参
	if ((RunstatusRegs.ReturnFlag.bit.q == 1) & (RunstatusRegs.Alarm.limit.all == 0))
	{
		if (RunstatusRegs.ret_finished_x == -1)
		{
			if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0001) == 0)
				RunstatusRegs.ret_finished_x = 1;
			else
			{
				RunstatusRegs.ReturnSpeed[0] = ReturnR_Axis(1, &RunstatusRegs.ret_finished_x, &PostionRegs.real_pos.pos_x, RunstatusRegs.ReturnSpeed[0]);
			}
			if (RunstatusRegs.ret_finished_x == 1)
			{
				PostionRegs.real_pos.pos_x = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[0];
				PostionRegs.real_posbk.pos_x = PostionRegs.real_pos.pos_x;
				RunstatusRegs.ReturnSpeed[0] = 0;
			}
		}

		if (RunstatusRegs.ret_finished_y == -1)
		{
			if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0002) == 0)
				RunstatusRegs.ret_finished_y = 1;
			else
			{
				RunstatusRegs.ReturnSpeed[1] = ReturnR_Axis(2, &RunstatusRegs.ret_finished_y, &PostionRegs.real_pos.pos_y, RunstatusRegs.ReturnSpeed[1]);
			}
			if (RunstatusRegs.ret_finished_y == 1)
			{
				PostionRegs.real_pos.pos_y = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[1];
				PostionRegs.real_posbk.pos_y = PostionRegs.real_pos.pos_y;
				RunstatusRegs.ReturnSpeed[1] = 0;
			}
		}

		if (RunstatusRegs.ret_finished_z == -1)
		{
			if ((ParamterUnRegs.ParamterRegs.MAxisparmRegs.MachZero & 0x0004) == 0)
				RunstatusRegs.ret_finished_z = 1;
			else
			{
				RunstatusRegs.ReturnSpeed[2] = ReturnR_Axis(3, &RunstatusRegs.ret_finished_z, &PostionRegs.real_pos.pos_z, RunstatusRegs.ReturnSpeed[2]);
			}
			if (RunstatusRegs.ret_finished_z == 1)
			{
				PostionRegs.real_pos.pos_z = ParamterUnRegs.ParamterRegs.AxisparmRegs.RetRefCoord[2];
				PostionRegs.real_posbk.pos_z = PostionRegs.real_pos.pos_z;
				RunstatusRegs.ReturnSpeed[2] = 0;
			}
		}
		if (RunstatusRegs.ret_finished_x == 1 && RunstatusRegs.ret_finished_y == 1 && RunstatusRegs.ret_finished_z == 1)
		{
			RunstatusRegs.RunStatusFlag.bit.CBAFlag = 0;
			RunstatusRegs.ReturnFlag.bit.q = 0;
		}
	}
}
// 自动运行
uint16_t M30request = 0, brakeFinish = 0, headMrequest = 0, tailMrequest = 0;
uint16_t cmdAutoReset = 1;
uint16_t AutoRun(void)
{
	static uint16_t phase = 0; //,phA=0,segEnd=0;
	uint16_t cmdRun = 0;

	cmdRun = RunstatusRegs.RunStatusFlag.bit.AutoFlag;
	if (cmdAutoReset == 1)
	{
		cmdAutoReset = 0;
		phase = 0;
	}
	switch (phase)
	{
	case 0:
		if ((RunstatusRegs.RunStatusFlag.bit.CBAFlag) && cmdRun)
		// means have valid G-Code in the As reg.
		{
			if (EncodeRegs.AsReg.M == 30)
			{
				M30request = 1;
				phase = 99; // means run code finished
			}
			else if (EncodeRegs.AsReg.HaveMCode) // need segment head pause
			{
				headMrequest = 1;
				phase = 1;
			}
			else
			{
				int i;
				for (i = 0; i < 16; i++)
					EncodeRegs.AsReg.MCode[i] = 0;
				phase = 2; // goto run the G code
			}
		}
		break;
	case 1: // wait for execute head Mcode
		if (cmdRun == 0)
			phase = 2;
		break;
	case 2: // start run the G code
		if ((RunstatusRegs.RunStatusFlag.bit.CBAFlag) && cmdRun)
		// means have valid G-Code in the As reg.
		{
			if (EncodeRegs.AsReg.Type == 0)
				phase = 5; //
			else if (EncodeRegs.AsReg.Type == 1)
				phase = 7;
			else if ((EncodeRegs.AsReg.Type == 2) || (EncodeRegs.AsReg.Type == 3))
				phase = 9;
			//          else if(EncodeRegs.AsReg.Type == 2)
			//              phase = 9;
			//          else if(EncodeRegs.AsReg.Type == 3)
			//              phase = 10;
		}
		break;
	//-----------------------------------------------------
	case 5:
		if (MyRpdRun2(1) == 0) // 本行 G code 执行完毕
			phase = 20;		   // goto execute tail M code
		else if (cmdRun == 0)
			phase = 6; // goto pause
		break;
	case 6:					// deal the pause event
		if (RpdRun(0) == 0) // pause complete then jump;
			phase = 12;		// goto G code pause
		break;
	//----------------------------------------------------
	case 7:
		//      JointsChabu();
		//        if(RunstatusRegs.RunStatusFlag.bit.CBAFlag == 0)
		//            phase = 20;
		if (LineChabu(1) == 0) // 本行执行完毕
			phase = 20;		   // goto execute tail M code
		else if (cmdRun == 0)
			phase = 8; // goto pause
		break;
	case 8:
		if (LineChabu(0) == 0) // pause complete then jump;
			phase = 12;		   // goto G code pause
		break;
	//----------------------------------------------------
	case 9:
		//      Shun_Cir();
		//      if(RunstatusRegs.RunStatusFlag.bit.CBAFlag == 0)
		//          phase = 20;
		if (ShunNi_Cir(1) == 0) // 本行执行完毕
			phase = 20;			// goto execute tail M code
		else if (cmdRun == 0)
			phase = 10; // goto pause
		break;
	case 10:
		//      Ni_Cir();
		//        if(RunstatusRegs.RunStatusFlag.bit.CBAFlag == 0)
		//            phase = 20;
		if (ShunNi_Cir(0) == 0) // pause complete then jump;
			phase = 12;			// goto G code pause
		break;
	//----------------------------------------------------
	case 12: // Gcode brake finished
		brakeFinish = 1;
		phase = 2;
		break;
	case 20:
		if (EncodeRegs.AsReg.HaveTCode || EncodeRegs.AsReg.P) // need segment tail pause
		{
			tailMrequest = 1;
			phase = 21;
		}
		else
		{
			int i;
			for (i = 0; i < 16; i++)
				EncodeRegs.AsReg.MCode[i] = 0;
			phase = 22; // goto run next line
		}

		break;
	case 21: // wait for execute tail Mcode
		if (cmdRun == 0)
			phase = 22;
		break;
	case 22: // this line head_Mcode->Gcode->tail_Mcode have finished;
		if (cmdRun == 1)
		{
			uint16_t i;
			for (i = 0; i < 16; i++)
				EncodeRegs.AsReg.MCode[i] = 0;

			//            RunstatusRegs.RunStatusFlag.bit.CBAFlag=0;
			if (ThreeWordsReg.PosNumX == 0 && ThreeWordsReg.PosNumY == 0 && ThreeWordsReg.PosNumZ == 0 &&
				ThreeWordsReg.PosNumA == 0 && ThreeWordsReg.PosNumB == 0 && ThreeWordsReg.PosNumC == 0 &&
				ThreeWordsReg.PosNumW == 0 && ThreeWordsReg.PosNumU == 0)
				RunstatusRegs.RunStatusFlag.bit.DownProgramFlag = 0; // 刘天翼测试用
			phase = 0;
		}
		break;
	case 99: // means have finished all the M G M code, meet M30
		RunstatusRegs.ProNum = 0;
		if (cmdRun == 0) // RunstatusRegs.RunStatusFlag.bit.AutoFlag == 0)
			phase = 0;
		break;
	default:
		phase = 0;
	}

	return 0;
}

//*************************************************************************
