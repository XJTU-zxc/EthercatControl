/*
 * rapidMove.h
 *
 *  Created on: 2015-5-13
 *      Author: Administrator
 */

#ifndef RAPIDMOVE_H_
#define RAPIDMOVE_H_

#include "accSint.h"
#include "variabledef.h"

class CrapidMove
{
public:
	enum STATUS status;

	struct IN
	{	float      		*ptSetV;
		float  			*ptSetA;
		float			*ptSetJ;
		struct RunReg 	*ptAsReg;
		uint16_t   		cmdRun;
		struct inter_pos posi;
		unsigned int 	valid;
	} in;


	struct VIEW
	{	struct inter_pos posi;
		unsigned int	valid;
	} out;

private:
	uint16_t phase;
	CaccSint accX,accY,accZ,accA,accB,accC;

public:
	CrapidMove(){ in.valid=0; phase=0;}
	void reset(void);
	void sureInput(void);
	void run(void);
};//endof call define

#endif /* RAPIDMOVE_H_ */
