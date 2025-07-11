/*
 * accSint.cpp
 *
 *  Created on: 2014-12-1
 *      Author: Administrator
 */
#include "math.h"

#include "accSint.h"

void CaccSint::reset(void)
{
	obj.reset();
	bakPosition = 0;
	status = idle;
	in.pauseCmd = 0;
	in.setV = 0.000001;
	in.length = 10;
	in.valid = 0;
}

void CaccSint::sureInput(void)
{
	if (in.length < 0)
		dir = -1;
	else
		dir = 1;
	ullength = in.length * dir;
	obj.in.flength = (float)ullength;

	//	if (in.setV<0.00001)
	//		obj.in.setV=0.00001;
	//	else
	obj.in.setV = in.setV;
	obj.in.setA = in.setA;
	obj.in.setJ = in.setJ;

	obj.sureInput();
	status = BUSY;
	in.valid = 1;
}

void CaccSint::run()
{
	long temp;

	if (in.valid != 1)
	{
		status = END;
		return;
	}
	obj.in.pauseCmd = in.pauseCmd;

	obj.run();
	view.v = obj.view.v;
	temp = long(obj.view.f_posi + 0.5);
	if (bakPosition > temp)
		temp = bakPosition;
	else
		bakPosition = temp;

	if (temp > ullength)
		temp = ullength;

	view.position = temp * dir;
	status = obj.status;
}
