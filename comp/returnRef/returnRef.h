/*
 * returnRef.h
 *
 *  Created on: 2014-12-16
 *      Author: Administrator
 */

#ifndef RETURNREF_H_
#define RETURNREF_H_

#include "variabledef.h"
//#include "comp/accSFloat2/accSFloat2.h"

class CreturnRef
{
public:
	enum STATUS status;

	struct IN
	{	long		position;
		float  		setVrapid;
		float 		setVslow;
		float  		setAcc;
		float		setJ;
		float		ZeroShift;//unit is m
		int   		dir;//
		unsigned int 	valid;
	} in;


	struct VIEW
	{	long 		  position;
		unsigned int	valid;
	} view;

private:
	//long  posiBak;
	float deltaT;
	float v,T1,T2,t;
	int phase;
	float f_posi;

public:
	CreturnRef(){ deltaT=0.004; in.valid=0; }
	void reset(void);
	void sureInput(void);
	void run(uint8_t zeroSW,uint8_t index);

};


#endif /* RETURNREF_H_ */
