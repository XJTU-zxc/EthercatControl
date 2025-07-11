/*
 * jog.cpp
 *
 *  Created on: 2014-12-10
 *      Author: Administrator
 */
#include "jog.h"
#include "math.h"
#define deltaT 0.004
//*****************************************************
void Cjog::reset(void)
{	f_posi=0.0;
	posiBak=0;
	v=0.0;
	a=0.0;
	in.setV=0.2;
	in.setAcc=0.008;//加速度
	in.setJ=3.0;//加加速度
	in.cmdHiBrake=0;
	in.HiBrakeValue=5;
	status=idle;
}
void Cjog::sureInput(void)
{	in.valid=1;
	status=BUSY;
	//calculate T1 T2, judge if T2=0;
	if (in.setV<0.000001)
		in.setV=0.000001;

	if (in.setJ<0.001)
		in.setJ=0.001;

	if (in.setAcc<0.001)
		in.setAcc=0.001;

	float Vmin;

	/* 加加速段时间：T1 = a / j */
	T1=in.setAcc/in.setJ;

	/* 计算加加速阶段到达的速度：vmin = a * T1 */
	Vmin=in.setAcc*T1;

	if (in.setV>Vmin)
		T2=(in.setV-Vmin)/in.setAcc;
	else
	{	T2=0;
		T1=sqrt(in.setV/in.setJ);
	}
	t=0;

	/* v01 = 0.5JT^2 */
	V01=in.setJ*T1*T1*(1.0/2.0);
	/* V02 = V01 + J*T1*T2 */
	/* V = V01 + A*T2 */
	V02=V01+in.setJ*T1*T2;
}

void Cjog::calBrake(void)
{	phaseBrake=0;
	if (t<=T1)
	{	T1=t;
		T2=0;
	}
	else if(t<T1+T2)
	{	T2=t-T1;
	}
}
void Cjog::runUp(void)
{	if(in.valid!=1) return;

/*
	if (v<in.setV)
	{	v+=in.setAcc*0.004;
		if (v>in.setV)
			v=in.setV;
	}
*/


	t += deltaT;//0.004;

//	if(t <= T1)
//	{	a=in.setJ*t;
//	}
//	else if(t <= T1 + T2 )
//	{	//float x;
//		//x=t - T1;
//		a=in.setJ*T1;
//	}
//	else if(t < T1 + T2 + T1 )
//	{	float x;
//		x=t - T1 - T2;
//		if ( (T1-x)<=0.0 )
//			a=0.0;
//		else
//			a=in.setJ*(T1-x);
//	}
//	else //if(t < T1 + T2 + T3 + T4 )
//	{	a=0.0;
//	}

	if (a>in.setAcc)
	{
		a=in.setAcc;
	}

	v+=a*deltaT;//0.004;
	if (v>in.setV)
		v=in.setV;


	if(t <= T1)
	{	v=in.setJ*t*t*(1.0/2.0);
	}
	else if(t <= T1 + T2 )
	{	float x;
		x=t - T1;
		v=V01+in.setJ*T1*x;
	}
	else if(t < T1 + T2 + T1 )
	{	float x;
		x=t - T1 - T2;
	   	v=V02+in.setJ*T1*x-0.5*in.setJ*x*x;
	}
	else //if(t < T1 + T2 + T3 + T4 )
	{	v=in.setV;
	}



	f_posi+=v*deltaT;//0.004;

	long temp;
	temp=(long)(f_posi*1000000.0+0.5);
	view.delta=temp-posiBak;
	posiBak=temp;
	status=BUSY;
}
void Cjog::runDown(void)
{
	switch(phaseBrake)
	{	case 0:
			if (a>0.0)
				a-=in.setJ*deltaT;//0.004;
			else
			{	t=0.0;
				phaseBrake=1;
			}
			status=BUSY;
		break;
		case 1:
			t+=deltaT;//0.004;
			if(t <= T1)
			{	a=-in.setJ*t;
			}
			else if(t <= T1 + T2 )
			{	//float x;
				//x=t - T1;
				a=-in.setJ*T1;
			}
			else if(t < T1 + T2 + T1 )
			{	float x;
				x=t - T1 - T2;
				if ( (T1-x)<=0.0 )
					a=0.0;
				else
					a=-in.setJ*(T1-x);
			}
			else //if(t < T1 + T2 + T3 + T4 )
			{	a=0.0;
				v=0.0;
				phaseBrake=2;
			}
			status=BUSY;
		break;
		case 2:
			//status=END;
		break;
	}

	v+=a*deltaT;//0.004;

	if (in.cmdHiBrake)// in order for the rapid brake stop
		v-=(float)in.HiBrakeValue *in.setAcc*deltaT;


	//v-=in.setAcc*0.004;

	if (v<=0.0)
	{	view.delta=0;
		status=END;
		return;
	}

	f_posi+=v*deltaT;//0.004;
	long temp;
	temp=(long)(f_posi*1000000.0+0.5);
	view.delta=temp-posiBak;
	posiBak=temp;

}