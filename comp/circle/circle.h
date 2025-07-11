/*
 * circle.h
 *
 *  Created on: 2014-12-2
 *      Author: Administrator
 */
#include "variabledef.h"
#include "accSFloat2.h"

#ifndef CIRCLE_H_
#define CIRCLE_H_

class Ccircle
{
public:
	enum STATUS status;

	struct IN
	{	long   		startX;
		long   		startY;
		long   		endX;
		long   		endY;
		long 		cenX;
		long		cenY;
		float 		setV;
		float       setA;
		float		setJ;
		int   		dir;//shun yuan=2    niyuan=3
		uint16_t 		pauseCmd;
		uint16_t	 	valid;
	} in;


	struct VIEW
	{	long			pos_x;
		long			pos_y;
		float			v;
		unsigned int	valid;
	} view;

private:

	CaccSFloat2   obj;
	float r;
	float stArc;
	float endArc;
	float arcLen;

//	float	t;
//	float   deltaT;
//	float   Vmax;
//	float   Amax;
//	float   J;
public:
	Ccircle(){ in.valid=0; }
	void reset(void);
	void sureInput(void);
	void run(void);
	void reStart(void);

};//endof call define



#endif /* CIRCLE_H_ */
