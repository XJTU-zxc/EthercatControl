/*
 * accLSFloat.cpp
 *
 *  Created on: 2014-12-1
 *      Author: Administrator
 */
#include "math.h"

#include "accLSFloat.h"

#define deltaT 0.004
//*************************************************************************
void CaccLSFloat::reset()
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
float CaccLSFloat::SLength(float Vset)
{
	float V1, V2, pT1, pT2, S1max, S2max, S3max, Sall;
	pT1 = Amax / J;
	V1 = J * pT1 * pT1 * (1.0 / 2.0);
	if (V1 <= Vset * (1.0 / 2.0)) //% A_reach_first
	{
		pT2 = (Vset - J * pT1 * pT1) / Amax;
		V2 = V1 + J * pT1 * pT2;
		S1max = J * pT1 * pT1 * pT1 * (1.0 / 6.0);
		S2max = V1 * pT2 + J * pT1 * pT2 * pT2 * (1.0 / 2.0);
		S3max = V2 * pT1 + J * pT1 * pT1 * pT1 * (1.0 / 3.0);
		Sall = S1max + S2max + S3max;
	}
	else //% V_reach_first
	{
		pT2 = 0;
		pT1 = sqrt(Vset / J);
		Sall = J * pT1 * pT1 * pT1;
	}
	return Sall;
}
//------------------------------------------------------
void CaccLSFloat::calT5T6T7(float Vset)
{
	float pT1, pT2, V1;
	pT1 = Amax / J;
	V1 = J * pT1 * pT1 * (1.0 / 2.0);
	if (V1 <= Vset * (1.0 / 2.0)) //% A_reach_first
		pT2 = (Vset - J * pT1 * pT1) / Amax;
	else //% V_reach_first
	{
		pT2 = 0;
		pT1 = sqrt(Vset / J);
	}
	T7 = T5 = pT1;
	T6 = pT2;
}
//------------------------------------------------------
void CaccLSFloat::sureInput()
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
	//-----------------------------------------------------
	// this acc first half use line acc, last half use S acc
	// so T1=0, T3=0,  T5=0;
	//  T2!=0   T4not sure  T6!not sure   T7!=0;
	// step 1 should calculate S_T7max,  then T2max-->T6-->S_T2max_T6
	// step 2 if Ssum>S_T7max+S_T2max_T6  means T4>0;
	//       calculate T4 -->end
	// step 3 else if Ssum>S_T7max+S_T2min, means T4=0; T2>0 T6>=0
	//					--
	// step 4 else T4=0;T6=0;T2>0,T7>0;need try a V-->T2-->T6
	//----------------------------------------------------
	{
		float SLS; //,V2;
		uint16_t i;

		Sum = fabs(in.flength) * 0.000001; // /1000000.0;
		// status=BUSY;
		view.f_posi = in.pause_fPosition; // 0.0
		// in.valid=0;
		// first judge if have T4
		SLS = Vmax * Vmax / (2.0 * Amax) + SLength(Vmax);
		if (Sum > SLS) // means have T4, T1=T3=0.0;
		{
			T4 = (Sum - SLS) / Vmax;
			T1 = T3 = 0.0;
			T2 = Vmax / Amax;
			calT5T6T7(Vmax);
		}
		else // T4=0;T1=T3=0.0;
		{
			float Vup, Vdown, Vnew, St2, Serror;
			T1 = T3 = T4 = 0.0;
			Vup = Vmax;
			Vdown = 0.0;
			for (i = 0; i < 40; i++)
			{
				Vnew = (Vup - Vdown) * 0.7 + Vdown;
				St2 = Vnew * Vnew / Amax * (1.0 / 2.0);
				Serror = Sum - St2 - SLength(Vnew);
				if (fabs(Serror) < 0.0000003)
					break;
				else
				{
					if (Serror < 0.0) //>(Sum<St2+St7)
						Vup = Vnew;
					else
						Vdown = Vnew;
				}
			}
			T2 = Vnew / Amax;
			calT5T6T7(Vnew);
		}

		V01 = 0.0;
		V02 = Amax * T2;
		V03 = V02;
		V04 = V03;
		V05 = V04 - J * T5 * T5 * (1.0 / 2.0);
		V06 = V05 - J * T5 * T6;
		V07 = V06 - J * T7 * T7 * (1.0 / 2.0);

		S01 = 0.0;
		S02 = S01 + V02 * T2 * (1.0 / 2.0);
		S03 = S02;
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
void CaccLSFloat::run()
{
	switch (phase)
	{
	case 0:
		//	GpioDataRegs.GPADAT.bit.GPIOA9=1;
		t += deltaT;

		if (t <= T2)
		{
			float x, s;
			x = t;
			s = Amax * x * x * (1.0 / 2.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V01 + Amax * x;
		}
		else if (t < T2 + T4)
		{
			float x, s;
			x = t - T2;
			s = S03 + V03 * x;
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V03;
		}
		else if (t < T2 + T4 + T5)
		{
			float x, s;
			x = t - T2 - T4;
			s = S04 + V04 * x - J * x * x * x * (1.0 / 6.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V04 - (1.0 / 2.0) * J * x * x;
		}
		else if (t < T2 + T4 + T5 + T6)
		{
			float x, s;
			x = t - T2 - T4 - T5;
			s = S05 + V05 * x - J * T5 * x * x * (1.0 / 2.0);
			view.f_posi = in.pause_fPosition + s * 1000000.0;
			view.v = V05 - J * T5 * x;
		}
		else if (t < T2 + T4 + T5 + T6 + T7)
		{
			float x, s;
			x = t - T2 - T4 - T5 - T6;
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
		if (in.pauseCmd) //==1 or ==2 need brake
		{
			phase = 1;
		}

		break;
	case 1:
		if (in.pauseCmd == 2)
			view.v -= Amax * deltaT * 5;
		else
			view.v -= Amax * deltaT;

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
	default:
		phase = 0;
	} // end of switch
} // end of void run()

//***********************************************************
