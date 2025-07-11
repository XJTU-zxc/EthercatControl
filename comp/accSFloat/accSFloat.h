

#include "variabledef.h"

#ifndef accSFloat_H
#define accSFloat_H

class CaccSFloat
{
public:
	enum STATUS status;

	struct IN
	{	long   			length;
		unsigned int 	valid;
	} in;


	struct VIEW
	{
		long			position;
		float 			f_posi;
		float			v;
		float           a;
		unsigned int	valid;
	} view;

private:
 	unsigned int phase;
	int remain,dir;
	long preViewPosition;

	//float 	distance;
	float 	Sum;
	float   T1;
	float	T2;
	float   T3;
	float   T4;
	float   T5;
	float	T6;
	float	T7;

	float	V01;
	float	V02;
	float	V03;
	float	V04;
	float	V05;
	float	V06;
	float	V07;

	float	S01;
	float	S02;
	float	S03;
	float	S04;
	float	S05;
	float	S06;
	float	S07;

	float	t;
	float   deltaT;
	float   Vmax;
	float   Amax;
	float   J;
public:
	CaccSFloat(){ in.valid=0; }
	void reset(void);
	void sureInput(void);
	void run(void);

};//endof call define

#endif
