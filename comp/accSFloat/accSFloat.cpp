

#include "math.h"

#include "accSFloat.h"
//*************************************************************************
void CaccSFloat::reset()
{ 	status=idle;
	phase=0;
	dir=1;
	in.length=10;
	in.valid=0;
	view.f_posi=0.0;
	view.position=0;
	preViewPosition=0;
	T1=0.0;
	T2=0.0;
	deltaT=0.004;//1ms
	Vmax=0.89;
	Amax=129.309;
	J=0.000322;//0.00122;
}
//------------------------------------------------------
void CaccSFloat::sureInput()
{
	remain=in.length;
	in.valid=1;
}
//-----------------------------------------------------
void CaccSFloat::run()
{
	switch(phase)
	{	case 0:

			if (in.valid==1)
			{	float Sa,Sb,Sc,S1max,S2max,S3max,V1,V2;
				uint16_t type=0;
				if (in.length<0)
					dir=-1;
				else dir=1;

				Sum=(float)(in.length*dir)*0.000001;    // /1000000.0;
				status=BUSY;
				view.f_posi=0;

				in.valid=0;
			//----------------------------------
			// if T4=0,T1,T2,T3,T5,T6,T7!=0;

		       	T1=Amax/J;
		       	V1=J*T1*T1/2.0;
		       	if ( V1 <= Vmax/2.0   ) //% A_reach_first
		       	{
			       T2=( Vmax-J*T1*T1 ) / Amax;
			       V2=V1+J*T1*T2;
				   S1max=J*T1*T1*T1/6.0;
				   S2max=V1*T2+J*T1*T2*T2/2.0;
				   S3max=V2*T1+J*T1*T1*T1/3.0;
			       Sa=(S1max+S2max+S3max)*2.0;
			       if (Sum>=Sa)
			            type=0;//%T2>0 T4>0
			       else
			       {   Sb=J*T1*T1*T1*2.0;
			           if (Sum>=Sb)
			              type=1;//%T2>0 T4=0
			           else
			              type=3;//%T2=0;T4=0
			       }
			    }
			    else //% V_reach_first
			    {   T2=0;
			        T1=sqrt(Vmax/J);
			        Sc=J*T1*T1*T1*2.0;
			        if (Sum>=Sc)
			           type=2;//%T2=0;T4>0
			        else
			           type=3;//%T2=0;T4=0
			       	}

			    if (type==0)  //%T2 fix value  T4>0
			    {   T1=Amax/J;
			        T2=(Vmax-J*T1*T1 ) / Amax;
			        T4=(Sum-Sa)/Vmax;
			        T3=T1;
			        T5=T1;
			        T6=T2;
			        T7=T1;
			    }
			    else if (type==1)     //%T2>0 T4=0
			    {   float delta;
			   		T1=Amax/J;
			        delta=J*T1*(J*T1*T1*T1+4*Sum);
			        T2=(-3.0*J*T1*T1+sqrt(delta))/(2.0*J*T1);
			        T4=0;
			        T3=T1;
			        T5=T1;
			        T6=T2;
			        T7=T1;
			   	}
			   	else if (type==2) //%T2=0;T4>0
			    {   T1=sqrt(Vmax/J);
			      	T2=0;
			        T3=T1;
			        T4=(Sum-Sc)/Vmax;
				    T5=T1;
			        T6=T2;
			        T7=T1;
			    }
			    else //%type==3   %T2=0;T4=0
			    {  	float x;
					x=Sum/(2.0*J);
				    T1=exp(log(x)/3.0);//(op->Sum/(2.0*op->J))^(1.0/3.0);
				    T2=0.0;
				    T3=T1;
				    T4=0.0;
				    T5=T1;
				    T6=0.0;
				    T7=T1;
			   	}


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
				t=0.0;
				phase=1;
			}
		break;
		case 1:
		//	GpioDataRegs.GPADAT.bit.GPIOA9=1;

			t += deltaT;


			if(t <= T1)
			{	float x,s;
				x=t;
				s=J*x*x*x/6.0;
				view.f_posi= s*1000000.0*(float)dir;
			}
			else if(t <= T1 + T2 )
			{	float x,s;
				x=t - T1;
				s= S01+V01*x+J * T1*x*x/2.0;
				view.f_posi= s * 1000000.0*(float)dir;
			}
			else if(t < T1 + T2 + T3 )
			{
				float x,s;
				x=t - T1 - T2;
	        	s=S02+V02*x+J*T1*x*x/2.0-J*x*x*x/6.0;
	        	view.f_posi=s*1000000.0*(float)dir;
			}
			else if(t < T1 + T2 + T3 + T4 )
			{
				float x,s;
				x=t - T1 - T2 - T3;
				s=S03+V03*x;
				view.f_posi=s*1000000.0*(float)dir;
			}
			else if(t < T1 + T2 + T3 + T4 + T5 )
			{
				float x,s;
				x=t - T1 - T2 - T3- T4;
				s=S04 + V04*x - J*x*x*x/6.0;
				view.f_posi=s*1000000.0*(float)dir;
			}
			else if(t < T1 + T2 + T3 + T4 + T5 + T6 )
			{
				float x,s;
				x=t - T1 - T2 - T3- T4- T5;
				s=S05+V05*x-J*T5*x*x/2.0;
				view.f_posi=s*1000000.0*(float)dir;
			}
			else if(t < T1 + T2 + T3 + T4 + T5 + T6 + T7 )
			{
				float x,s;
				x=t - T1 - T2 - T3- T4- T5- T6;
				s=S06+V06*x-J*T5*x*x/2.0+J*x*x*x/6.0;
				view.f_posi=s*1000000.0*(float)dir;
			}
			else
			{
				view.f_posi=(float)in.length;//->distance;
				status=END;
				phase=0;
			}

		//	GpioDataRegs.GPADAT.bit.GPIOA9=0;

		break;
		default:	phase=0;
	}//end of switch


	if (view.f_posi<0.0)
		view.position=(long)(view.f_posi-0.5);
	else
		view.position=(long)(view.f_posi+0.5);


	if (dir==1)
	{	if (view.position<preViewPosition)
			view.position=preViewPosition;
		else preViewPosition=view.position;
		if (view.position>in.length)
			view.position=in.length;
	}
	else //(dir==-1)
	{	if (view.position>preViewPosition)
			view.position=preViewPosition;
		else preViewPosition=view.position;
		if (view.position<in.length)
			view.position=in.length;
	}

}//end of void run()

//***********************************************************
