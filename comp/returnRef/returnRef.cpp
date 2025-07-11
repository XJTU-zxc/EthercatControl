/*
 * returnRef.cpp
 *
 *  Created on: 2014-12-16
 *      Author: Administrator
 */
#include  "returnRef.h"
#include "math.h"
void CreturnRef::reset(void)
{	in.setVrapid=0.01;
	in.setVslow=0.001;
	phase=0;
	f_posi=0;
	in.valid=0;
	status=idle;
}

void CreturnRef::sureInput(void)
{	status=BUSY;
	in.valid=1;
}

void CreturnRef::run(uint8_t zeroSW,uint8_t index)
{	switch(phase)
	{
	case 0:
		v=0.0;
		if (in.valid==1)
		{	phase=1;
			in.valid=0;
		}
		else break;
	//break;
	case 1:// move to zero position
		v+=in.setAcc*deltaT;
		if (v>=in.setVrapid)
			v=in.setVrapid;

		f_posi+=v*deltaT*in.dir;
		if (zeroSW)
			phase=2;//need brake to zero speed
	break;
	case 2:
		v-=in.setAcc*deltaT;
		if (v<0.0)
		{ 	v=0.0;
			phase=3;//brake finish
		}
		else
			f_posi+=v*deltaT*in.dir;
	break;
	case 3:// backward run
		v+=in.setAcc*deltaT;
		if (v>=in.setVslow)
			v=in.setVslow;

		f_posi-=v*deltaT*in.dir;
		if (zeroSW)// ����λ������
			phase=4;//
	break;
	case 4:
		v+=in.setAcc*deltaT;
		if (v>=in.setVslow)
			v=in.setVslow;

		f_posi-=v*deltaT*in.dir;
		if (zeroSW==0)// ������λ����
			phase=5;//
	break;
	case 5:
		f_posi-=v*deltaT*in.dir;
		if (index)	//�ҵ�һת
		{	phase=6;

			//status=END;
		}
	break;
	case 6://move fix distance
			//calculate T1 T2 T3
		{	float T1max,S1max;

			T1max=in.setVrapid / in.setAcc;
			S1max=in.setAcc * T1max * T1max*0.5;  //   / 2.0;

			if(in.ZeroShift >= 2.0 * S1max)
			{	T1=T1max;
				T2=(in.ZeroShift - 2.0 * S1max) / in.setVrapid;
			}
			else
			{	T1=sqrt(in.ZeroShift / in.setAcc);
				T2=0.0;
			}
		}
		t=0.0;
		phase=7;
	//break;
	case 7:
		t += deltaT;

		if(t <= T1)
			v=in.setAcc*t;
		else if(t <= T1 + T2 )
			v=in.setAcc*T1;
		else if(t < 2.0 * T1 + T2 )
			v=in.setAcc*(2.0*T1+T2-t);
		else
			phase=8;

		f_posi-=v*deltaT*in.dir;
	break;
	case 8:
		status=END;
		phase=0;
	break;
	default: phase=0;
	}

	view.position=(long)(f_posi*1000000.0+0.5);

}













