/*
 * rapidMove.cpp
 *
 *  Created on: 2015-5-13
 *      Author: Administrator
 */
#include "rapidMove.h"


void CrapidMove::reset(void)
{	phase=0;
	accX.reset();
	accY.reset();
	accZ.reset();
	accA.reset();
	accB.reset();
	accC.reset();
}

void CrapidMove::sureInput(void)
{	accX.in.length=(*in.ptAsReg).X - in.posi.pos_x;
	accX.in.setV=*in.ptSetV++;//
	accX.in.setA=*in.ptSetA++;
	accX.in.setJ=*in.ptSetJ++;
	accX.sureInput();


	accY.in.length=(*(in.ptAsReg)).Y - in.posi.pos_y;
	accY.in.setV=*in.ptSetV++;//
	accY.in.setA=*in.ptSetA++;
	accY.in.setJ=*in.ptSetJ++;
	accY.sureInput();


	accZ.in.length=(*(in.ptAsReg)).Z - in.posi.pos_z;
	accZ.in.setV=*in.ptSetV++;//
	accZ.in.setA=*in.ptSetA++;
	accZ.in.setJ=*in.ptSetJ++;
	accZ.sureInput();

	accA.in.length=(*(in.ptAsReg)).A - in.posi.pos_a;
	accA.in.setV=*in.ptSetV++;//
	accA.in.setA=*in.ptSetA++;
	accA.in.setJ=*in.ptSetJ++;
	accA.sureInput();

	accB.in.length=(*(in.ptAsReg)).B - in.posi.pos_b;
	accB.in.setV=*in.ptSetV++;//
	accB.in.setA=*in.ptSetA++;
	accB.in.setJ=*in.ptSetJ++;
	accB.sureInput();

	accC.in.length=(*(in.ptAsReg)).C - in.posi.pos_c;
	accC.in.setV=*in.ptSetV++;//
	accC.in.setA=*in.ptSetA++;
	accC.in.setJ=*in.ptSetJ++;
	accC.sureInput();
}

void CrapidMove::run(void)
{	switch(phase)
	{
	case 0:
		accX.run();
		out.posi.pos_x=in.posi.pos_x+accX.view.position;

		accY.run();
		out.posi.pos_y=in.posi.pos_y+accY.view.position;

		accZ.run();
		out.posi.pos_z=in.posi.pos_z+accZ.view.position;

		accA.run();
		out.posi.pos_a=in.posi.pos_a+accA.view.position;

		accB.run();
		out.posi.pos_b=in.posi.pos_b+accB.view.position;

		accC.run();
		out.posi.pos_c=in.posi.pos_c+accC.view.position;

		if( (accX.status==END)&&(accY.status==END)&&(accZ.status==END)
				  &&(accA.status==END)&&(accB.status==END)&&(accC.status==END)
		  )
		{	//RunstatusRegs.RunStatusFlag.bit.CBAFlag=0;//send flag to load AS next G code.
			phase=1;// can deal another line G code.
			//busy=0;//means not busy, can do other things.
		}
		else
		{//	if(cmdRun==0)//need brake
			{	accX.in.pauseCmd=1;
				accY.in.pauseCmd=1;
				accZ.in.pauseCmd=1;
				accA.in.pauseCmd=1;
				accB.in.pauseCmd=1;
				accC.in.pauseCmd=1;
				phase=2;
			}
			//busy=1;//means busy.
		}

	break;
	case 1:
		status=END;

	break;
	case 2:




	break;
	default:	;
	}

}
