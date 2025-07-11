/*
 * accSFloat2.cpp
 *
 *  Created on: 2015-01-19
 *      Author: Administrator
 */
#include "math.h"
#include "accSFloatVs.h"
//*************************************************************************
void CaccSFloatVs::reset()
{ 	status=idle;
	in.flength=10.0;
	in.valid=0;
	in.Vs=0;
	in.Ve=0;
	in.setV=0.0089;
	in.setA=0.209;
	in.setJ=0.322;//0.00122;

	view.f_posi=0.0;
	T1=T2=T3=0.0;

	t=0.0;

}
//------------------------------------------------------
void CaccSFloatVs::calSa()
{	float V1,V2,S1max,S2max,S3max;
	T1=Amax/J;
	if (Vmax>J*T1*T1+in.Vs)//means T2>0;
	{//then cal Sa=S1+S2+S3
		T2=( Vmax-J*T1*T1-in.Vs ) / Amax;
		V1=in.Vs+0.5*J*T1*T1;
		V2=V1+J*T1*T2;
	}
	else//T2=0;
	{	T2=0.0;
		T1=sqrt((Vmax-in.Vs)/J);
		V1=in.Vs+0.5*J*T1*T1;
		V2=V1;
	}
	T3=T1;
	S1max=in.Vs*T1+J*T1*T1*T1*(1.0/6.0);
	S2max=V1*T2+J*T1*T2*T2*(1.0/2.0);
	S3max=V2*T1+J*T1*T1*T1*(1.0/3.0);
	Sa=S1max+S2max+S3max;
}
void CaccSFloatVs::calSb()
{	float V1R,V2R,T1R,T2R,S1maxR,S2maxR,S3maxR;
	T1R=Amax/J;
	if (Vmax>J*T1R*T1R+in.Ve)//means T2R>0;
	{//then cal Sb=S1R+S2R+S3R
		T2R=( Vmax-J*T1R*T1R-in.Ve ) / Amax;
		V1R=in.Ve+0.5*J*T1R*T1R;
		V2R=V1R+J*T1R*T2R;
	}
	else//T2R=0;
	{	T2R=0.0;
		T1R=sqrt((Vmax-in.Ve)/J);
		V1R=in.Ve+0.5*J*T1R*T1R;
		V2R=V1R;
	}
	T5=T1R;
	T6=T2R;
	T7=T1R;
	S1maxR=in.Ve*T1R+J*T1R*T1R*T1R*(1.0/6.0);
	S2maxR=V1R*T2R+J*T1R*T2R*T2R*(1.0/2.0);
	S3maxR=V2R*T1R+J*T1R*T1R*T1R*(1.0/3.0);
	Sb=S1maxR+S2maxR+S3maxR;
}
void CaccSFloatVs::sureInput()
{
	J=in.setJ;
	Amax=in.setA;

	if (in.setV<0.00001)
		Vmax=0.00001;
	else
		Vmax=in.setV;
	in.valid=1;

	status=BUSY;

	Sum=in.flength*0.000001;    // /1000000.0;
	//-------------------------------------------------------
			// if T4=0,T1,T2,T3,T5,T6,T7!=0;
		//step1 suppose T4>=0, then cal out  min sum
	      //step2 cal Sa
			calSa();
	    //step3 cal Sb
			calSb();

	       	if (Sum>Sa+Sb)//T4>0
	       	{	T4=(Sum-Sa-Sb)/Vmax;
	       	}
	       	else
	       	{	//T4=0; need reduce the Vmax
	       		int i;
	       		float Vup,Vdown,Vnew;

	       		T4=0;

	       		Vup=Vmax;
	       		Vdown=0;


	       		for(i=0;i<20;i++)
	       		{	Vnew=(Vup-Vdown)*0.7+Vdown;
					Vmax=Vnew;
					calSa();
					calSb();

					if (fabs(Sum-Sa-Sb)<0.000001)
						break;
					else
					{	if (Sum<Sa+Sb)
							Vup=Vmax;
						else
							Vdown=Vmax;
					}
	       		}
	       	}

	 //-----------------------------------------------------------------------
	 //-----------------------------------------------------------------------
			V01=J*T1*T1/2.0;
			V02=V01+J*T1*T2;
			V03=V02+J*T3*T3/2.0;
			V04=V03;
			V05=V04-J*T5*T5/2.0;
			V06=V05-J*T5*T6;
			V07=V06-J*T7*T7/2.0;


			S01=J*T1*T1*T1/6.0;
			S02=S01+V01*T2+J*T1*T2*T2/2.0;
			S03=S02+V02*T3+J*T1*T1*T1/3.0;
			S04=S03+V03*T4;
			S05=S04+V04*T5-J*T5*T5*T5/6.0;
			S06=S05+V05*T6-J*T5*T6*T6/2.0;
			S07=S06+V06*T7-J*T7*T7*T7/3.0;

			//------------------------------------
		    if((T2<0)||(T4<0))
		    	while(1);

}
//-----------------------------------------------------
void CaccSFloatVs::run()
{
			t += deltaT;

			if(t <= T1)
			{	float x,s;
				x=t;
				s=J*x*x*x/6.0;
				view.f_posi= s*1000000.0;
				view.v=J*x*x/2.0;
			}
			else if(t <= T1 + T2 )
			{	float x,s;
				x=t - T1;
				s= S01+V01*x+J * T1*x*x/2.0;
				view.f_posi= s * 1000000.0;
				view.v=V01+J*T1*x;
			}
			else if(t < T1 + T2 + T3 )
			{
				float x,s;
				x=t - T1 - T2;
	        	s=S02+V02*x+J*T1*x*x/2.0-J*x*x*x/6.0;
	        	view.f_posi=s*1000000.0;
	        	view.v=V02+J*T1*x-0.5*J*x*x;
			}
			else if(t < T1 + T2 + T3 + T4 )
			{
				float x,s;
				x=t - T1 - T2 - T3;
				s=S03+V03*x;
				view.f_posi=s*1000000.0;
				view.v=V03;
			}
			else if(t < T1 + T2 + T3 + T4 + T5 )
			{
				float x,s;
				x=t - T1 - T2 - T3- T4;
				s=S04 + V04*x - J*x*x*x/6.0;
				view.f_posi=s*1000000.0;
				view.v=V04-0.5*J*x*x;
			}
			else if(t < T1 + T2 + T3 + T4 + T5 + T6 )
			{
				float x,s;
				x=t - T1 - T2 - T3- T4- T5;
				s=S05+V05*x-J*T5*x*x/2.0;
				view.f_posi=s*1000000.0;
				view.v=V05-J*T5*x;
			}
			else if(t < T1 + T2 + T3 + T4 + T5 + T6 + T7 )
			{
				float x,s;
				x=t - T1 - T2 - T3- T4- T5- T6;
				s=S06+V06*x-J*T5*x*x/2.0+J*x*x*x/6.0;
				view.f_posi=s*1000000.0;
				view.v=V06-J*T5*x+0.5*J*x*x;
			}
			else
			{
				view.f_posi=Sum;//->distance;
				view.v=0.0;
				status=END;
			}
}//end of void run()

//***********************************************************




