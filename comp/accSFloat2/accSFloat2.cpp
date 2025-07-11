/*
 * accSFloat2.cpp
 *
 *  Created on: 2014-12-1
 *      Author: Administrator
 */
#include "math.h"
#include "variabledef.h"
#include "accSFloat2.h"

#define deltaT 0.004
//*************************************************************************
void CaccSFloat2::reset()
{
	status = idle;
	phase = 0;
	in.pauseCmd = 0;
	in.pause_fPosition = 0.0;
	in.flength = 10.0;
	in.valid = 0;
	in.setV = 0.0089;
	in.setA = 0.209;
	in.setJ = 0.322; // 0.00122;

	view.f_posi = 0.0; // in.pause_fPosition;//0.0;
	T1 = 0.0;
	T2 = 0.0;
	//	deltaT=0.004;//1ms
	// Vmax=0.0089;//0.89
	//	Amax=129.309;
	//	J=0.0000322;//0.00122;
}
//------------------------------------------------------
void CaccSFloat2::sureInput()
{
	// remain=in.length;

	//	J=in.setJ;
	//	Amax=in.setA;

	if (in.setV < 0.00001)
		Vmax = 0.00001;
	else
		Vmax = in.setV;

	if (in.setA < 0.001)
		Amax = 0.001;
	else
		Amax = in.setA;

	if (in.setJ < 0.001)
		J = 0.001;
	else
		J = in.setJ;

	status = BUSY;
	in.valid = 1;
	//----------------------------------------------------
	{
		float Sa, Sb, Sc, S1max, S2max, S3max, V1, V2;
		uint16_t type = 0;

		Sum = fabs(in.flength) * 0.000001; // /1000000.0;
		// status=BUSY;
		view.f_posi = in.pause_fPosition; // 0.0

		// in.valid=0;
		//----------------------------------
		//  if T4=0,T1,T2,T3,T5,T6,T7!=0;

		T1 = Amax / J;
		V1 = J * T1 * T1 * (1.0 / 2.0);
		if (V1 <= Vmax * (1.0 / 2.0)) //% A_reach_first
		{
			T2 = (Vmax - J * T1 * T1) / Amax;
			V2 = V1 + J * T1 * T2;
			S1max = J * T1 * T1 * T1 * (1.0 / 6.0);
			S2max = V1 * T2 + J * T1 * T2 * T2 * (1.0 / 2.0);
			S3max = V2 * T1 + J * T1 * T1 * T1 * (1.0 / 3.0);
			Sa = (S1max + S2max + S3max) * 2.0;
			if (Sum >= Sa)
				type = 0; //%T2>0 T4>0
			else
			{
				Sb = J * T1 * T1 * T1 * 2.0;
				if (Sum >= Sb)
					type = 1; //%T2>0 T4=0
				else
					type = 3; //%T2=0;T4=0
			}
		}
		else //% V_reach_first
		{
			T2 = 0;
			T1 = sqrt(Vmax / J);
			Sc = J * T1 * T1 * T1 * 2.0;
			if (Sum >= Sc)
				type = 2; //%T2=0;T4>0
			else
				type = 3; //%T2=0;T4=0
		}

		if (type == 0) //%T2 fix value  T4>0
		{
			T1 = Amax / J;
			T2 = (Vmax - J * T1 * T1) / Amax;
			T4 = (Sum - Sa) / Vmax;
			T3 = T1;
			T5 = T1;
			T6 = T2;
			T7 = T1;
		}
		else if (type == 1) //%T2>0 T4=0
		{
			float delta;
			T1 = Amax / J;
			delta = J * T1 * (J * T1 * T1 * T1 + 4.0 * Sum);
			T2 = (-3.0 * J * T1 * T1 + sqrt(delta)) / (2.0 * J * T1);
			T4 = 0;
			T3 = T1;
			T5 = T1;
			T6 = T2;
			T7 = T1;
		}
		else if (type == 2) //%T2=0;T4>0
		{
			T1 = sqrt(Vmax / J);
			T2 = 0;
			T3 = T1;
			T4 = (Sum - Sc) / Vmax;
			T5 = T1;
			T6 = T2;
			T7 = T1;
		}
		else //%type==3   %T2=0;T4=0
		{
			float x;
			x = Sum / (2.0 * J);
			// T1=exp(log(x)/3.0);//(op->Sum/(2.0*op->J))^(1.0/3.0);
			T1 = pow(x, 1.0 / 3.0);
			T2 = 0.0;
			T3 = T1;
			T4 = 0.0;
			T5 = T1;
			T6 = 0.0;
			T7 = T1;
		}

		V01 = J * T1 * T1 * (1.0 / 2.0);
		V02 = V01 + J * T1 * T2;
		V03 = V02 + J * T3 * T3 * (1.0 / 2.0);
		V04 = V03;
		V05 = V04 - J * T5 * T5 * (1.0 / 2.0);
		V06 = V05 - J * T5 * T6;
		V07 = V06 - J * T7 * T7 * (1.0 / 2.0);

		S01 = J * T1 * T1 * T1 * (1.0 / 6.0);
		S02 = S01 + V01 * T2 + J * T1 * T2 * T2 * (1.0 / 2.0);
		S03 = S02 + V02 * T3 + J * T1 * T1 * T1 * (1.0 / 3.0);
		S04 = S03 + V03 * T4;
		S05 = S04 + V04 * T5 - J * T5 * T5 * T5 * (1.0 / 6.0);
		S06 = S05 + V05 * T6 - J * T5 * T6 * T6 * (1.0 / 2.0);
		S07 = S06 + V06 * T7 - J * T7 * T7 * T7 * (1.0 / 3.0);

		//------------------------------------
		if ((T2 < 0) || (T4 < 0))
			while (1)
				;
		t = 0.0;
		// phase=0;
	}
}
//-----------------------------------------------------
void CaccSFloat2::run()
{
	switch (phase)
	{
	case 0:
		//	GpioDataRegs.GPADAT.bit.GPIOA9=1;
		t += deltaT;
		// printf("[INFO] deltaT = %f\n", deltaT);
		if (t <= T1)
		{
			float x, s;
			x = t;
			s = J * x * x * x * (1.0 / 6.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = J * x * x * (1.0 / 2.0);
		}
		else if (t <= T1 + T2)
		{
			float x, s;
			x = t - T1;
			s = S01 + V01 * x + J * T1 * x * x * (1.0 / 2.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V01 + J * T1 * x;
		}
		else if (t < T1 + T2 + T3)
		{
			float x, s;
			x = t - T1 - T2;
			s = S02 + V02 * x + J * T1 * x * x * (1.0 / 2.0) - J * x * x * x * (1.0 / 6.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V02 + J * T1 * x - (1.0 / 2.0) * J * x * x;
		}
		else if (t < T1 + T2 + T3 + T4)
		{
			float x, s;
			x = t - T1 - T2 - T3;
			s = S03 + V03 * x;
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V03;
		}
		else if (t < T1 + T2 + T3 + T4 + T5)
		{
			float x, s;
			x = t - T1 - T2 - T3 - T4;
			s = S04 + V04 * x - J * x * x * x * (1.0 / 6.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V04 - (1.0 / 2.0) * J * x * x;
		}
		else if (t < T1 + T2 + T3 + T4 + T5 + T6)
		{
			float x, s;
			x = t - T1 - T2 - T3 - T4 - T5;
			s = S05 + V05 * x - J * T5 * x * x * (1.0 / 2.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V05 - J * T5 * x;
		}
		else if (t < T1 + T2 + T3 + T4 + T5 + T6 + T7)
		{
			float x, s;
			x = t - T1 - T2 - T3 - T4 - T5 - T6;
			s = S06 + V06 * x - J * T5 * x * x * (1.0 / 2.0) + J * x * x * x * (1.0 / 6.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V06 - J * T5 * x + (1.0 / 2.0) * J * x * x;
		}
		else
		{
			view.f_posi = in.pause_fPosition + Sum * 1000000.0; //->distance;
			view.v = 0.0;
			status = END;
			in.valid = 0;
			phase = 0;
		}

		//	GpioDataRegs.GPADAT.bit.GPIOA9=0;
		//	if(in.pauseCmd)//==1 or ==2//need brake
		//	{	phase=1;
		//	}
		if (in.pauseCmd == 1) // need normal brake
		{
			if (t + deltaT < T1 + T2 + T3 + T4) // need brake immediatly
			{
				if (t <= T1)
				{
					T1 = t;
					T2 = 0.0;
					T3 = T1;
					// t=0.0;
					phase = 3;
				}
				else if (t < T1 + T2)
				{
					T2 = t - T1;
					T3 = T1;
					// t=0.0;
					phase = 4;
				}
				else if (t < T1 + T2 + T3)
				{ //	phase=5;
				}
				else
				{
					t = T1 + T2 + T3;
					// phase=6;
				}
				phase = 3;

				// t=0.0;
			}
			else
				phase = 2; // already enter brake,can do nothing
		}
		else if ((in.pauseCmd == 2) && (status != END)) // need hard brake
		{
			phase = 1;
		}
		break;
	case 1:
		// if (in.pauseCmd==2)
		view.v -= Amax * deltaT * 5;
		if (view.v > 0.0)
		{
			view.f_posi += view.v * deltaT * 1000000.0;
		}
		else
		{
			status = END;
			in.valid = 0;
			phase = 0;
		}
		break;
	case 2: // already enter brake,can do nothing

		t += deltaT;

		if (t < T1 + T2 + T3 + T4 + T5)
		{
			float x, s;
			x = t - T1 - T2 - T3 - T4;
			s = S04 + V04 * x - J * x * x * x * (1.0 / 6.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V04 - (1.0 / 2.0) * J * x * x;
		}
		else if (t < T1 + T2 + T3 + T4 + T5 + T6)
		{
			float x, s;
			x = t - T1 - T2 - T3 - T4 - T5;
			s = S05 + V05 * x - J * T5 * x * x * (1.0 / 2.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V05 - J * T5 * x;
		}
		else if (t < T1 + T2 + T3 + T4 + T5 + T6 + T7)
		{
			float x, s;
			x = t - T1 - T2 - T3 - T4 - T5 - T6;
			s = S06 + V06 * x - J * T5 * x * x * (1.0 / 2.0) + J * x * x * x * (1.0 / 6.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V06 - J * T5 * x + (1.0 / 2.0) * J * x * x;
		}
		else
		{
			view.f_posi = in.pause_fPosition + Sum * 1000000.0; //->distance;
			view.v = 0.0;
			status = END;
			in.valid = 0;
			phase = 0;
		}
		if ((in.pauseCmd == 2) && (status != END)) // need hard brake
		{
			phase = 1;
		}
		break;
	case 3:
		t += deltaT;
		if (t < 2.0 * T1 + T2)
		{
			float x;
			x = 2.0 * T1 + T2 - t;
			view.v = J * T1 * T1 + J * T1 * T2 - 0.5 * J * x * x;
		}
		else if (t < 3.0 * T1 + T2)
		{
			float x;
			x = t - (2.0 * T1 + T2);
			view.v = J * T1 * T1 + J * T1 * T2 - 0.5 * J * x * x;
		}
		else if (t < 3.0 * T1 + 2.0 * T2)
		{
			float x;
			x = t - (3.0 * T1 + T2);
			view.v = 0.5 * J * T1 * T1 + J * T1 * (T2 - x);
		}
		else if (t < 4.0 * T1 + 2.0 * T2)
		{
			float x;
			x = 4.0 * T1 + 2.0 * T2 - t;
			view.v = 0.5 * J * x * x;
		}
		else
			view.v = 0.0;

		if (view.v > 0.0)
		{
			view.f_posi += view.v * deltaT * 1000000.0;
		}
		else
		{
			status = END;
			in.valid = 0;
			phase = 0;
		}
		if ((in.pauseCmd == 2) && (status != END)) // need hard brake
		{
			phase = 1;
		}
		break;

	default:
		phase = 0;
	}
}

//***********************************************************
