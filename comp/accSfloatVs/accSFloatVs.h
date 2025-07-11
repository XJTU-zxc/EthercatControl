/*
 * accSFloatVs.h
 *
 *  Created on: 2015-01-19
 *      Author: Administrator
 */
#include "variabledef.h"

#ifndef ACCSFLOATVS_H_
#define ACCSFLOATVS_H_

class CaccSFloatVs
{
public:
	enum STATUS status;

	struct IN
	{	float  			flength;
		float			Ve;
		float			Vs;
		float			setV;
		float			setA;
		float			setJ;
		unsigned int 	valid;
	} in;


	struct VIEW
	{
		//long			position;
		float 			f_posi;//positive
		float			v;
		float           a;
		unsigned int	valid;
	} view;

private:
 	float   Sa;
 	float   Sb;
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


	void calSa(void);
	void calSb(void);
public:
	CaccSFloatVs(void)
	{ 	in.valid=0;
		deltaT=0.004;//ms }
	}

	void reset(void);
	void sureInput(void);
	void run(void);

};//endof call define



#endif /* ACCSFLOATVS_H_ */
