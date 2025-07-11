/*
 * circle.cpp
 *
 *  Created on: 2014-12-2
 *      Author: Administrator
 */
#include "circle.h"
#include "math.h"
#define PI 3.1415926535
//*****************************************************
void Ccircle::reset(void)
{	obj.reset();
	in.startX=10;
	in.startY=0;
	in.endX=10;
	in.endY=1;
	in.cenX=0;
	in.cenY=0;
	in.setV=0.001;
	in.dir=2;//shun yuan
	status=idle;
	in.pauseCmd=0;
	arcLen=0.0;
	in.valid=0;

}
//**********************************************
void Ccircle::sureInput(void)
{	float fdx,fdy,arcRange;

	if ((in.startX==in.cenX)&&(in.startY==in.cenY))
	{	r=0.0;
		return;
	}
	fdx=(float)(in.startX-in.cenX);
	fdy=(float)(in.startY-in.cenY);
	r=sqrt(fdx*fdx+fdy*fdy);

	if (in.startY>=in.cenY)
		stArc=atan2(fdy,fdx);
	else
		stArc=atan2(fdy,fdx)+2.0*PI;


	if ((in.startX==in.endX)&&(in.startY==in.endY)) //globle circle
	{	obj.in.flength=2.0*PI*r;
		//	endArc=StArc;
	}
	else//not a globle circle
	{	fdx=(float)(in.endX-in.cenX);
		fdy=(float)(in.endY-in.cenY);

		if (in.endY>=in.cenY)
			endArc=atan2(fdy,fdx);
		else
			endArc=atan2(fdy,fdx)+2.0*PI;

		if (in.dir==2)	//shun yuan
		{	if (stArc>=endArc)
			{	arcRange=stArc-endArc;
			}
			else
			{	arcRange=2.0*PI+stArc-endArc;
				if (arcRange<0.0)
					arcRange=0.0;
			}
		}
		else if (in.dir==3)//ni yuan
		{	if(endArc>=stArc)
			{	arcRange=endArc-stArc;
			}
			else
			{	arcRange=2.0*PI+endArc-stArc;
				if (arcRange<0.0)
					arcRange=0.0;
			}
		}
		obj.in.flength=arcRange*r;
	}
	arcLen=obj.in.flength;
	obj.in.setV=in.setV;
	obj.in.setA=in.setA;
	obj.in.setJ=in.setJ;
	obj.sureInput();
	in.valid=1;
	status=BUSY;
}
//************************************************
void Ccircle::run(void)
{	float ftemp,arc1,arc2;
	long ltemp;

	if ((r==0.0)||(in.valid!=1))
	{	status=END;
		return;
	}
	obj.in.pauseCmd=in.pauseCmd;
	obj.run();
	view.v=obj.view.v;
	arc1=obj.view.f_posi/r;

	if (in.dir==2)//shun yuan
	{	arc2=stArc-arc1;
	//	if (stArc>=arc1)
	//		arc2=stArc-arc1;
	//	else
	//		arc2=2.0*PI+stArc-arc1;
	}
	else if(in.dir==3) //ni yuan
	{	arc2=stArc+arc1;
		//arc2=stArc+arc1;
		//if (arc2>=2.0*PI)
		//	arc2-=2.0*PI;
	}
//-------------------------
	ftemp=r*cos(arc2);
	if (ftemp>=0.0)
		ltemp=(long)(ftemp+0.5);
	else
		ltemp=(long)(ftemp-0.5);
	view.pos_x=in.cenX+ltemp;
//--------------------
	ftemp=r*sin(arc2);
	if (ftemp>=0.0)
		ltemp=(long)(ftemp+0.5);
	else
		ltemp=(long)(ftemp-0.5);
	view.pos_y=in.cenY+ltemp;
//------------------
	status=obj.status;

}
//*************************************************
void Ccircle::reStart(void)
{
	// obj.view.f_posi  present position
	//	obj.in.flength  goal

		obj.in.pause_fPosition=obj.view.f_posi;
		obj.in.flength=arcLen - obj.view.f_posi;
		if (obj.in.flength<=0)
			obj.in.flength=0.0;

		//obj.in.pauseCmd=0;
		obj.in.setV=in.setV;
		obj.in.setA=in.setA;
		obj.in.setJ=in.setJ;
		obj.sureInput();
		in.valid=1;
		status=BUSY;
}
//*************************************************

