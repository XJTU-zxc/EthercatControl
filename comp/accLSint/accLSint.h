/*
 * accLSint.h
 *
 *  Created on: 2014-12-1
 *      Author: Administrator
 */
#include "variabledef.h"
#include "accLSFloat.h"
//#include "accSFloat.h"

#ifndef ACCLSINT_H_
#define ACCLSINT_H_


class CaccLSint
{
public:
	enum STATUS status;

	struct IN
	{	uint16_t			pauseCmd;
		long   			length;
		float			setV;
		float 			setA;
		float			setJ;
		unsigned int 	valid;
	} in;


	struct VIEW
	{
		long			position;
		float			v;
		float           a;
		unsigned int	valid;
	} view;

private:
	//class CaccFloat obj;
	CaccLSFloat obj;
	uint32_t ullength;
	int dir;
	long bakPosition;
	float	t;
	float   deltaT;
	float   Vmax;
	float   Amax;
	float   J;
public:
	CaccLSint(){ in.valid=0; }
	void reset(void);
	void sureInput(void);
	void run(void);

};//endof call define


#endif /* ACCLSINT_H_ */
