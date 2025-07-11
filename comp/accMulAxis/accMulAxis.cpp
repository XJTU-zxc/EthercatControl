/*
 * accMulAxis.cpp
 *
 *  Created on: 2014-11-28
 *      Author: Administrator
 */

#include "accMulAxis.h"
#include "math.h"
//*****************************************************
void CaccMulAxis::reset(void)
{	obj.reset();
	in.lengthA=0;
	in.lengthB=0;
	in.lengthC=0;
	in.lengthX=0;
	in.lengthY=0;
	in.lengthZ=0;
    in.lengthW=0;
    in.lengthU=0;
	in.pauseCmd=0;

	bakX=0;
	bakY=0;
	bakZ=0;
	bakA=0;
	bakB=0;
	bakC=0;
    bakW=0;
    bakU=0;

	status=idle;
	in.valid=0;
}
//**********************************************
void CaccMulAxis::sureInput(void)
{	float flength,fx,fy,fz,fa,fb,fc,fu,fw,ftemp;


	if (in.lengthX<0) dirX=-1; else dirX=1;
	if (in.lengthY<0) dirY=-1; else dirY=1;
	if (in.lengthZ<0) dirZ=-1; else dirZ=1;
	if (in.lengthA<0) dirA=-1; else dirA=1;
	if (in.lengthB<0) dirB=-1; else dirB=1;
	if (in.lengthC<0) dirC=-1; else dirC=1;
    if (in.lengthW<0) dirW=-1; else dirW=1;
    if (in.lengthU<0) dirU=-1; else dirU=1;

	fx=(float)(in.lengthX*dirX);
	fy=(float)(in.lengthY*dirY);
	fz=(float)(in.lengthZ*dirZ);
	fa=(float)(in.lengthA*dirA);
	fb=(float)(in.lengthB*dirB);
	fc=(float)(in.lengthC*dirC);
    fw=(float)(in.lengthW*dirW);
    fu=(float)(in.lengthU*dirU);

	if (  (in.lengthX==0)&&(in.lengthY==0)&&(in.lengthZ==0)
		&&(in.lengthA==0)&&(in.lengthB==0)&&(in.lengthC==0)&&(in.lengthW==0)&&(in.lengthU==0)
	   )
	{	kx=0.0;
		ky=0.0;
		kz=0.0;
		ka=0.0;
		kb=0.0;
		kc=0.0;
        kw=0.0;
        ku=0.0;
		obj.in.flength=0.0;
		obj.sureInput();
	}
	else
	{	flength=sqrt(fx*fx+fy*fy+fz*fz+fa*fa+fb*fb+fc*fc+fw*fw+fu*fu);
		ftemp=1.0/flength;
		kx=fx*ftemp;
		ky=fy*ftemp;
		kz=fz*ftemp;
		ka=fa*ftemp;
		kb=fb*ftemp;
		kc=fc*ftemp;
        kw=fw*ftemp;
        ku=fu*ftemp;
		obj.in.flength=flength;
		obj.in.setV=in.setV;
		obj.in.setA=in.setA;
		obj.in.setJ=in.setJ;
		obj.sureInput();
		status=BUSY;
	}
}
//************************************************
void CaccMulAxis::run(void)
{	long temp;

	obj.in.pauseCmd=in.pauseCmd;
	obj.run();
	view.v=obj.view.v;
//---------------
	temp=long(obj.view.f_posi*kx+0.5);
	if (temp<bakX)
		temp=bakX;
	else bakX=temp;

	if (temp>in.lengthX*dirX)
		temp=in.lengthX*dirX;
	view.pos_x=temp*dirX;
//---------------
	temp=long(obj.view.f_posi*ky+0.5);
	if (temp<bakY)
		temp=bakY;
	else bakY=temp;

	if (temp>in.lengthY*dirY)
		temp=in.lengthY*dirY;
	view.pos_y=temp*dirY;
//---------------
	temp=long(obj.view.f_posi*kz+0.5);
	if (temp<bakZ)
		temp=bakZ;
	else bakZ=temp;

	if (temp>in.lengthZ*dirZ)
		temp=in.lengthZ*dirZ;
	view.pos_z=temp*dirZ;
//---------------
	temp=long(obj.view.f_posi*ka+0.5);
	if (temp<bakA)
		temp=bakA;
	else bakA=temp;

	if (temp>in.lengthA*dirA)
		temp=in.lengthA*dirA;
	view.pos_a=temp*dirA;
//---------------
	temp=long(obj.view.f_posi*kb+0.5);
	if (temp<bakB)
		temp=bakB;
	else bakB=temp;

	if (temp>in.lengthB*dirB)
		temp=in.lengthB*dirB;
	view.pos_b=temp*dirB;
//---------------
	temp=long(obj.view.f_posi*kc+0.5);
	if (temp<bakC)
		temp=bakC;
	else bakC=temp;

	if (temp>in.lengthC*dirC)
		temp=in.lengthC*dirC;
	view.pos_c=temp*dirC;
//------------------
    temp=long(obj.view.f_posi*kw+0.5);
    if (temp<bakW)
        temp=bakW;
    else bakW=temp;

    if (temp>in.lengthW*dirW)
        temp=in.lengthW*dirW;
    view.pos_w=temp*dirW;
//------------------
    temp=long(obj.view.f_posi*ku+0.5);
    if (temp<bakU)
        temp=bakU;
    else bakU=temp;

    if (temp>in.lengthU*dirU)
        temp=in.lengthU*dirU;
    view.pos_u=temp*dirU;
//------------------
	status=obj.status;
}
//*************************************************










