#include "variabledef.h"
#include "math.h"

// ---------------------------------------------------------------------------
// 函数功能：   Dd
// 输入参数：   无..
// 输出参数：   无
// 创建时间：   2006.03.24
// 最后修改时间:2006.03.24
// 修改原因：   
// 创建人  ：   杨晓龙
// ---------------------------------------------------------------------------
uint16_t DD(void)    // 点动
{
  	// 进给步长选择
	uint16_t DdSel = RunstatusRegs.DdSel & 0xfff;
	if ( DdSel == 1 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是+X轴
	{
		PostionRegs.real_pos.pos_x = PostionRegs.real_pos.pos_x + MainAxisRegs.BeiLv;
  	} 
	if ( DdSel == 2 && (DdSel ^ RunstatusRegs.DdSelBk)  )// 如果是-X轴
	{
		PostionRegs.real_pos.pos_x = PostionRegs.real_pos.pos_x - MainAxisRegs.BeiLv;
	}
	if	(DdSel == 4 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是+Y轴
	{
		PostionRegs.real_pos.pos_y = PostionRegs.real_pos.pos_y + MainAxisRegs.BeiLv;
	}
	if (DdSel == 8 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是-Y轴
	{
		PostionRegs.real_pos.pos_y = PostionRegs.real_pos.pos_y - MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x10 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是+Z轴
	{
		PostionRegs.real_pos.pos_z = PostionRegs.real_pos.pos_z + MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x20 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是-Z轴
	{
		PostionRegs.real_pos.pos_z = PostionRegs.real_pos.pos_z - MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x40 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是+A轴
	{
		PostionRegs.real_pos.pos_a = PostionRegs.real_pos.pos_a + MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x80 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是-A轴
	{
		PostionRegs.real_pos.pos_a = PostionRegs.real_pos.pos_a - MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x100 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是+B轴
	{
		PostionRegs.real_pos.pos_b = PostionRegs.real_pos.pos_b + MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x200 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是-B轴
	{
		PostionRegs.real_pos.pos_b = PostionRegs.real_pos.pos_b - MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x400 && (DdSel ^ RunstatusRegs.DdSelBk)) // 如果是+C轴
	{
		PostionRegs.real_pos.pos_c = PostionRegs.real_pos.pos_c + MainAxisRegs.BeiLv;
	}
	if (DdSel == 0x800 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是-C轴
	{
		PostionRegs.real_pos.pos_c = PostionRegs.real_pos.pos_c - MainAxisRegs.BeiLv;
	}
    if (DdSel == 0x1000 && (DdSel ^ RunstatusRegs.DdSelBk)) // 如果是+W轴
    {
        PostionRegs.real_pos.pos_w = PostionRegs.real_pos.pos_w + MainAxisRegs.BeiLv;
    }
    if (DdSel == 0x2000 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是-W轴
    {
        PostionRegs.real_pos.pos_w = PostionRegs.real_pos.pos_w - MainAxisRegs.BeiLv;
    }
    if (DdSel == 0x4000 && (DdSel ^ RunstatusRegs.DdSelBk)) // 如果是+U轴
    {
        PostionRegs.real_pos.pos_u = PostionRegs.real_pos.pos_u + MainAxisRegs.BeiLv;
    }
    if (DdSel == 0x8000 && (DdSel ^ RunstatusRegs.DdSelBk) )// 如果是-U轴
    {
        PostionRegs.real_pos.pos_u = PostionRegs.real_pos.pos_u - MainAxisRegs.BeiLv;
    }
	RunstatusRegs.DdSelBk = DdSel & 0xffff;
	return 0;
}
// ---------------------------------------------------------------------------
// 函数功能：   JOG加速计算
// 输入参数：   无..
// 输出参数：   无
// 创建时间：
// 最后修改时间:
// 修改原因：
// 创建人  ：   付科
// ---------------------------------------------------------------------------
int32_t JOG_Speed_Up(int32_t Obj_Velo, int32_t velo)
{
	int32_t temp;
	float temp1, Jog_k;
//	spd_up_par = 0.05;
	if(!RPDAccDecRegs.spd_up_par)		// 无加减速
   		velo = Obj_Velo;
   	else					// 加速
   	{
		if(velo >= (Obj_Velo - SpeedRegs.Epslion))		// 结束判断
			velo = Obj_Velo;
		else											// 指数加速
		{
		   	Jog_k = 1;
   			Jog_k = Jog_k / RPDAccDecRegs.spd_up_par;
   			temp1 = SpeedRegs.Epslion;
   			temp1 = temp1 / Obj_Velo;
   			Jog_k = 1 - pow(temp1, Jog_k);
			temp1 = (Obj_Velo - velo) * Jog_k;
			temp = (int32_t)temp1;
			if(temp < SpeedRegs.Epslion)
				temp = SpeedRegs.Epslion;
			velo += temp;
		}
	}
	return velo;
}
// ---------------------------------------------------------------------------
// 函数功能：   JOG减速计算
// 输入参数：   无..
// 输出参数：   无
// 创建时间：
// 最后修改时间:
// 修改原因：
// 创建人  ：   付科
// ---------------------------------------------------------------------------
int32_t JOG_Speed_Down(int32_t Obj_Velo,int32_t velo)
{
	int32_t temp;
	float temp1, Jog_k;
	if(!RPDAccDecRegs.spd_up_par)		// 无加减速
		velo = 0;
	else
	{
	 	if(velo <= SpeedRegs.Epslion)
			velo = 0;
		else
		{
			Jog_k = 1;
   			Jog_k = Jog_k / RPDAccDecRegs.spd_up_par;
   			temp1 = SpeedRegs.Epslion;
   			temp1 = temp1 / Obj_Velo;
   			Jog_k = 1 - pow(temp1, Jog_k);
			temp1 = velo * Jog_k;
			temp = (int32_t)temp1;

			if(temp < SpeedRegs.Epslion)
				temp = SpeedRegs.Epslion;
			velo -= temp;
		}
	}
	return velo;
}
// ---------------------------------------------------------------------------
// 函数功能：   JOG
// 输入参数：   无..
// 输出参数：   无
// 创建时间：   2006.03.24
// 最后修改时间:2006.03.24
// 修改原因：
// 创建人  ：   杨晓龙
// Jog运动   	Jog轴选定,且其他轴速度为0时才可以进入
// ---------------------------------------------------------------------------
/*
void JOG(uint16_t Limit)
{
	uint16_t JogSel = JOGRegs.JogSel & 0xfff;
    if (   (JogSel == BIT0) ) //  (Alarm.limit.bit.xp == 1) &&(Alarm.softlimit.bit.xp==0)&&
    {
     	JOGRegs.Jog_dir = 1;
     	JOGRegs.Jvi.pos_x = JOG_Speed_Up(RPDAccDecRegs.UniVelo_X, JOGRegs.Jvi.pos_x);
     	PostionRegs.real_pos.pos_x += JOGRegs.Jvi.pos_x;
    }
    else if( (JogSel == BIT1) )// -X轴Jog运动(Alarm.limit.bit.xr == 1)&&(Alarm.softlimit.bit.xr == 0)&&
    {
    	JOGRegs.Jog_dir = -1;
    	JOGRegs.Jvi.pos_x = JOG_Speed_Up(RPDAccDecRegs.UniVelo_X, JOGRegs.Jvi.pos_x);
    	PostionRegs.real_pos.pos_x -= JOGRegs.Jvi.pos_x;
    }
    else if( (JogSel == BIT2) )// +Y轴Jog运动(Alarm.limit.bit.yp == 1) && (Alarm.softlimit.bit.yp == 0) &&
    {
    	JOGRegs.Jog_dir = 1;
		JOGRegs.Jvi.pos_y = JOG_Speed_Up(RPDAccDecRegs.UniVelo_Y, JOGRegs.Jvi.pos_y);
		PostionRegs.real_pos.pos_y += JOGRegs.Jvi.pos_y;
	}
    else if( (JogSel == BIT3) ) // -Y轴Jog运动(Alarm.limit.bit.yr == 1) && (Alarm.softlimit.bit.yr == 0) &&
    {
    	JOGRegs.Jog_dir = -1;
    	JOGRegs.Jvi.pos_y = JOG_Speed_Up(RPDAccDecRegs.UniVelo_Y, JOGRegs.Jvi.pos_y);
    	PostionRegs.real_pos.pos_y -= JOGRegs.Jvi.pos_y;
    }
    else if((JogSel == BIT4) )// +Z轴Jog运动(Alarm.limit.bit.zp == 1) && (Alarm.softlimit.bit.zp == 0) &&
    {
    	JOGRegs.Jog_dir = 1;
		JOGRegs.Jvi.pos_z = JOG_Speed_Up(RPDAccDecRegs.UniVelo_Z, JOGRegs.Jvi.pos_z);
		PostionRegs.real_pos.pos_z += JOGRegs.Jvi.pos_z;
	}
    else if((JogSel == BIT5) )// -Z轴Jog运动(Alarm.limit.bit.zr == 1) && (Alarm.softlimit.bit.zr == 0) &&
    {
    	JOGRegs.Jog_dir = -1;
		JOGRegs.Jvi.pos_z = JOG_Speed_Up(RPDAccDecRegs.UniVelo_Z, JOGRegs.Jvi.pos_z);
		PostionRegs.real_pos.pos_z -= JOGRegs.Jvi.pos_z;
	}
    else if( (JogSel == BIT6) ) // +A轴Jog运动(Alarm.limit.bit.ap == 1) && (Alarm.softlimit.bit.ap == 0) &&
    {
    	JOGRegs.Jog_dir = 1;
		JOGRegs.Jvi.pos_a = JOG_Speed_Up(RPDAccDecRegs.UniVelo_A, JOGRegs.Jvi.pos_a);
		PostionRegs.real_pos.pos_a += JOGRegs.Jvi.pos_a;
	}
    else if( (JogSel == BIT7) ) // -A轴Jog运动(Alarm.limit.bit.ar == 1) && (Alarm.softlimit.bit.ar == 0) &&
    {
    	JOGRegs.Jog_dir = -1;
		JOGRegs.Jvi.pos_a = JOG_Speed_Up(RPDAccDecRegs.UniVelo_A, JOGRegs.Jvi.pos_a);
		PostionRegs.real_pos.pos_a -= JOGRegs.Jvi.pos_a;
	}
    else if((JogSel == BIT8) ) // +B轴Jog运动(Alarm.limit.bit.bp == 1) && (Alarm.softlimit.bit.bp == 0) &&
    {
    	JOGRegs.Jog_dir = 1;
    	JOGRegs.Jvi.pos_b = JOG_Speed_Up(RPDAccDecRegs.UniVelo_B, JOGRegs.Jvi.pos_b);
    	PostionRegs.real_pos.pos_b += JOGRegs.Jvi.pos_b;
    }
    else if( (JogSel == BIT9) )// -B轴Jog运动(Alarm.limit.bit.br == 1) && (Alarm.softlimit.bit.br == 0) &&
   	{
    	JOGRegs.Jog_dir = -1;
    	JOGRegs.Jvi.pos_b = JOG_Speed_Up(RPDAccDecRegs.UniVelo_B, JOGRegs.Jvi.pos_b);
    	PostionRegs.real_pos.pos_b -= JOGRegs.Jvi.pos_b;
    }
    else if( (JogSel == BIT10) ) // +C轴Jog运动(Alarm.limit.bit.cp == 1) && (Alarm.softlimit.bit.cp == 0) &&
     {
  		JOGRegs.Jog_dir = 1;
    	JOGRegs.Jvi.pos_c = JOG_Speed_Up(RPDAccDecRegs.UniVelo_C, JOGRegs.Jvi.pos_c);
    	PostionRegs.real_pos.pos_c += JOGRegs.Jvi.pos_c;
    }
    else if( (JogSel == BIT11) )// -C轴Jog运动(Alarm.limit.bit.cr == 1) && (Alarm.softlimit.bit.cr == 0) &&
   	{
    	JOGRegs.Jog_dir = -1;
    	JOGRegs.Jvi.pos_c = JOG_Speed_Up(RPDAccDecRegs.UniVelo_C, JOGRegs.Jvi.pos_c);
    	PostionRegs.real_pos.pos_c -= JOGRegs.Jvi.pos_c;
    }
    if(  (     ((JogSel != 1) && (JOGRegs.Jog_dir == 1))
    		|| ((JogSel != 2) && (JOGRegs.Jog_dir == -1)))
    		&& JOGRegs.Jvi.pos_x)// X轴减速运动
    {
     	JOGRegs.Jvi.pos_x = JOG_Speed_Down(RPDAccDecRegs.UniVelo_X, JOGRegs.Jvi.pos_x);
     	PostionRegs.real_pos.pos_x = PostionRegs.real_pos.pos_x + ((int32_t)JOGRegs.Jog_dir * JOGRegs.Jvi.pos_x);
    }
    if(    (   ((JogSel != 3) && (JOGRegs.Jog_dir == 1))
    		|| ((JogSel != 4) && (JOGRegs.Jog_dir == -1)))
    		&& (JOGRegs.Jvi.pos_y ))    // Y轴减速运动
    {
     	JOGRegs.Jvi.pos_y = JOG_Speed_Down(RPDAccDecRegs.UniVelo_Y, JOGRegs.Jvi.pos_y);
    	PostionRegs.real_pos.pos_y = PostionRegs.real_pos.pos_y + ((int32_t)JOGRegs.Jog_dir * JOGRegs.Jvi.pos_y);
    }

    if(		(	((JogSel != 5) && (JOGRegs.Jog_dir == 1))
    		 || ((JogSel != 6) && (JOGRegs.Jog_dir == -1))) // Z轴减速运动
    		&& (JOGRegs.Jvi.pos_z)  )
    {
     	JOGRegs.Jvi.pos_z = JOG_Speed_Down(RPDAccDecRegs.UniVelo_Z, JOGRegs.Jvi.pos_z);
    	PostionRegs.real_pos.pos_z = PostionRegs.real_pos.pos_z + ((int32_t)JOGRegs.Jog_dir * JOGRegs.Jvi.pos_z);
    }

    if(	(	((JogSel != 7) && (JOGRegs.Jog_dir == 1))
    	 || ((JogSel != 8) && (JOGRegs.Jog_dir == -1)))
    	&& (JOGRegs.Jvi.pos_a))    // A轴减速运动
    {
     	JOGRegs.Jvi.pos_a = JOG_Speed_Down(RPDAccDecRegs.UniVelo_A, JOGRegs.Jvi.pos_a);
    	PostionRegs.real_pos.pos_a = PostionRegs.real_pos.pos_a + ((int32_t)JOGRegs.Jog_dir * JOGRegs.Jvi.pos_a);
    }

    if(	(	((JogSel != 9) && (JOGRegs.Jog_dir == 1))
    	 || ((JogSel != 10) && (JOGRegs.Jog_dir == -1)))
    	&& (JOGRegs.Jvi.pos_b))// B轴减速运动
    {
     	JOGRegs.Jvi.pos_b = JOG_Speed_Down(RPDAccDecRegs.UniVelo_B, JOGRegs.Jvi.pos_b);
    	PostionRegs.real_pos.pos_b = PostionRegs.real_pos.pos_b + ((int32_t)JOGRegs.Jog_dir * JOGRegs.Jvi.pos_b);
    }

    if(	(	((JogSel != 11) && (JOGRegs.Jog_dir == 11))
    	 || ((JogSel != 12) && (JOGRegs.Jog_dir == 12)))
    		&& (JOGRegs.Jvi.pos_c))   // C轴减速运动
    {
     	JOGRegs.Jvi.pos_c = JOG_Speed_Down(RPDAccDecRegs.UniVelo_C, JOGRegs.Jvi.pos_c);
      	PostionRegs.real_pos.pos_c = PostionRegs.real_pos.pos_c + ((int32_t)JOGRegs.Jog_dir * JOGRegs.Jvi.pos_c);
    }
}
*/


/*void POSSPEED_Calc(POSSPEED *p)
{
     long tmp;
     unsigned int pos16bval,temp1;
     _iq Tmp1,newp,oldp;
// **** Position calculation - mechanical and electrical motor angle  **** //
     p->DirectionQep = EQep1Regs.QEPSTS.bit.QDF;    // Motor direction: 0=CCW/reverse, 1=CW/forward

     pos16bval=(unsigned int)EQep1Regs.QPOSCNT;     // capture position once per QA/QB period
     p->theta_raw = pos16bval+ p->cal_angle;        // raw theta = current pos. + ang. offset from QA

     // The following lines calculate p->theta_mech ~= QPOSCNT/mech_scaler [current cnt/(total cnt in 1 rev.)]
     // where mech_scaler = 4000 cnts/revolution
     tmp = (long)((long)p->theta_raw*(long)p->mech_scaler);     // Q0*Q26 = Q26
     tmp &= 0x03FFF000;
     p->theta_mech = (int)(tmp>>11);                // Q26 -> Q15
     p->theta_mech &= 0x7FFF;

     // The following lines calculate p->elec_mech
     p->theta_elec = p->pole_pairs*p->theta_mech;  // Q0*Q15 = Q15
     p->theta_elec &= 0x7FFF;

// Check an index occurrence
     if (EQep1Regs.QFLG.bit.IEL == 1)
     {
        p->index_sync_flag = 0x00F0;
        EQep1Regs.QCLR.bit.IEL=1;                   // Clear interrupt flag
     }
//     High Speed Calcultation using QEP Position counter ****  //
// Check unit Time out-event for speed calculation:
// Unit Timer is configured for 100Hz in INIT function
    if(EQep1Regs.QFLG.bit.UTO==1)                    // If unit timeout (one 100Hz period)
    {
        //   Differentiator   /
        // The following lines calculate position = (x2-x1)/4000 (position in 1 revolution)
        pos16bval=(unsigned int)EQep1Regs.QPOSLAT;                // Latched POSCNT value
        tmp = (long)((long)pos16bval*(long)p->mech_scaler);       // Q0*Q26 = Q26
        tmp &= 0x03FFF000;
        tmp = (int)(tmp>>11);                                     // Q26 -> Q15
        tmp &= 0x7FFF;
        newp=_IQ15toIQ(tmp);
        oldp=p->oldpos;

        if (p->DirectionQep==0)                     // POSCNT is counting down
        {
            if (newp>oldp)
                Tmp1 = - (_IQ(1) - newp + oldp);    // x2-x1 should be negative
            else
            Tmp1 = newp -oldp;
        }
        else if (p->DirectionQep==1)                // POSCNT is counting up
        {
            if (newp<oldp)
            Tmp1 = _IQ(1) + newp - oldp;
            else
            Tmp1 = newp - oldp;                     // x2-x1 should be positive
        }

        if (Tmp1>_IQ(1))
            p->Speed_fr = _IQ(1);
        else if (Tmp1<_IQ(-1))
            p->Speed_fr = _IQ(-1);
        else
            p->Speed_fr = Tmp1;
// Update the electrical angle
        p->oldpos = newp;
// Change motor speed from pu value to rpm value (Q15 -> Q0)
        // Q0 = Q0*GLOBAL_Q => _IQXmpy(), X = GLOBAL_Q
        p->SpeedRpm_fr = _IQmpy(p->BaseRpm,p->Speed_fr);
        //=======================================
        EQep1Regs.QCLR.bit.UTO=1;                   // Clear interrupt flag
    }
// **** Low-speed computation using QEP capture counter **** //
    if(EQep1Regs.QEPSTS.bit.UPEVNT==1)                 // Unit position event
    {
        if(EQep1Regs.QEPSTS.bit.COEF==0)               // No Capture overflow
            temp1=(unsigned long)EQep1Regs.QCPRDLAT;   // temp1 = t2-t1
        else                                           // Capture overflow, saturate the result
            temp1=0xFFFF;

        p->Speed_pr = _IQdiv(p->SpeedScaler,(unsigned long)temp1);    // p->Speed_pr = p->SpeedScaler/temp1
        Tmp1=p->Speed_pr;

        if (Tmp1>_IQ(1))
            p->Speed_pr = _IQ(1);
        else
            p->Speed_pr = Tmp1;

        // Convert p->Speed_pr to RPM
        if (p->DirectionQep==0)                                 // Reverse direction = negative
            p->SpeedRpm_pr = -_IQmpy(p->BaseRpm,p->Speed_pr);   // Q0 = Q0*GLOBAL_Q => _IQXmpy(), X = GLOBAL_Q
        else                                                    // Forward direction = positive
            p->SpeedRpm_pr = _IQmpy(p->BaseRpm,p->Speed_pr);    // Q0 = Q0*GLOBAL_Q => _IQXmpy(), X = GLOBAL_Q
        EQep1Regs.QEPSTS.all=0x88;                  // Clear Unit position event flag
    }
} */
// ---------------------------------------------------------------------------
// 函数功能：   电子手轮
// 输入参数：
// 输出参数：
// 创建时间:
// 最后修改时间:
// 修改原因：
// 创建人  ：   付科
// ---------------------------------------------------------------------------
// volatile uint32_t ehand_num = 0;			// 电子手轮计数值
// volatile uint32_t ehand_num_bak = 0;		// 上次计数值
// uint16_t ehand()
// {
// 	int32_t  InCounter;
// 	ehand_num = (uint32_t)EQep1Regs.QPOSCNT;     // capture position once per QA/QB period
// 	InCounter = (ehand_num - ehand_num_bak)>>2;
// 	ehand_num_bak = ehand_num;
// 	uint16_t HandwheelFlag = RunstatusRegs.HandwheelFlag & 0x3ff;
//   	switch(HandwheelFlag)
//   	{
//   		case C28X_BIT0:		// X轴
//    		case C28X_BIT1:		// X轴
//  			PostionRegs.real_pos.pos_x = PostionRegs.real_pos.pos_x + InCounter*MainAxisRegs.BeiLv;
//   			break;
//   		case C28X_BIT2:		// Y轴
//    		case C28X_BIT3:		// Y轴
//  			PostionRegs.real_pos.pos_y = PostionRegs.real_pos.pos_y + InCounter*MainAxisRegs.BeiLv;
// 			break;
//   		case C28X_BIT4:		// Z轴
//   		case C28X_BIT5:		// Z轴
//   			PostionRegs.real_pos.pos_z = PostionRegs.real_pos.pos_z + InCounter*MainAxisRegs.BeiLv;
// 			break;
//   		case C28X_BIT6:		// A轴
//   		case C28X_BIT7:		// A轴
//   			PostionRegs.real_pos.pos_a = PostionRegs.real_pos.pos_a + InCounter*MainAxisRegs.BeiLv;
// 			break;
//   		case C28X_BIT8:		// B轴
//   		case C28X_BIT9:		// B轴
//   			PostionRegs.real_pos.pos_b = PostionRegs.real_pos.pos_b + InCounter*MainAxisRegs.BeiLv;
// 			break;
//   		case C28X_BIT10:		// C轴
//    		case C28X_BIT11:		// C轴
//  			PostionRegs.real_pos.pos_c = PostionRegs.real_pos.pos_c + InCounter*MainAxisRegs.BeiLv;
// 			break;
// 	}
//   	return 0;
// }