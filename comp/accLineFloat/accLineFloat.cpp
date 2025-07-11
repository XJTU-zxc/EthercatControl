#include "math.h"
#include "accLineFloat.h"
//**************************************************************
void CaccLineFloat::sureInput(void)
{ 	remain=in.length;
	in.valid=1;
}
//---------------------------------------
void CaccLineFloat::reset(void)
{ 	status=idle;
	phase=0;
	in.length=10;
	view.position=0;

	T1=0.0;
	T2=0.0;
	T1max=0.0;
	S1max=0.0;
	deltaT=0.004;//4ms
	Vmax=0.00220;
	A=0.0000233;
}
//------------------------------------------------
void CaccLineFloat::run()
{
	switch(phase)
	{	case 0:
		//	if (in.valid==1)
		//	{
	//	GpioG1DataRegs.GPADAT.bit.GPIO19=1;
				distance=in.length*0.000001;    // /1000000.0;
				in.valid=0;

				status=BUSY;
				view.position=0;

				T1max=Vmax / A;
				S1max=A * T1max * T1max*0.5;  //   / 2.0;

				if(distance >= 2.0 * S1max)
				{	T1=T1max;
					T2=(distance - 2.0 * S1max) / Vmax;
				}
				else
				{	T1=sqrt(distance / A);
					T2=0.0;
				}
	//		GpioG1DataRegs.GPADAT.bit.GPIO19=0;
				t=0.0;
				phase=1;
		//	}
		//break;
		case 1:

			t += deltaT;

			if(t <= T1)
			{	view.position=0.5 * A *	t *	t *1000000.0;
				view.v=A*t;
			}
			else if(t <= T1 + T2 )
			{	view.position=(0.5 * A * T1 * T1
								+ A * T1 * (t - T1))*1000000.0;
				view.v=A*T1;
			}
			else if(t < 2.0 * T1 + T2 )
			{
				float tao,S12,temp1,temp2;

				tao=t -	T1 - T2;
				S12=0.5 * A * T1 * T1 +  A * T1 * T2;
				temp1=A * T1 * tao;
				temp2=0.5 * A * tao * tao;
				view.position=(S12 + temp1 - temp2)*1000000.0;
			//	temp1+=1.0;
				view.v=A*(2.0*T1+T2-t);
			}
			else
			{	view.v=0;
				view.position=in.length;//->distance;
				status=END;
				//phase=0;
			}
			remain=in.length-view.position;

		//GpioDataRegs.GPADAT.bit.GPIOA9=0;

		break;
		default:	phase=0;
	}
}
//*********************************************************





