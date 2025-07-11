/*
 * accSFloat2.h
 *
 *  Created on: 2014-12-1
 *      Author: Administrator
 */
#include "variabledef.h"

#ifndef ACCLSFLOAT_H_
#define ACCLSFLOAT_H_

class CaccLSFloat
{
public:
	enum STATUS status;

	struct IN
	{
		uint16_t pauseCmd;
		float pause_fPosition;
		float flength;
		float setV;
		float setA;
		float setJ;
		unsigned int valid;
	} in;

	struct VIEW
	{
		// long			position;
		float f_posi; // positive
		float v;
		float a;
		unsigned int valid;
	} view;

private:
	unsigned int phase;
	// int remain,dir;
	// long preViewPosition;

	// float 	distance;
	float Sum;
	float T1, T2, T3, T4, T5, T6, T7;
	float V01, V02, V03, V04, V05, V06, V07;
	float S01, S02, S03, S04, S05, S06, S07;
	float t;
	float deltaT;
	float Vmax;
	float Amax;
	float J;

public:
	CaccLSFloat() { in.valid = 0; }
	void reset(void);
	void sureInput(void);
	void run(void);
	void reStart(void);
	float SLength(float Vset);
	void calT5T6T7(float Vset);

}; // endof call define

#endif /* ACCLSFLOAT_H_ */
