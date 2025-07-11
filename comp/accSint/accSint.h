/*
 * accSint.h
 *
 *  Created on: 2014-12-1
 *      Author: Administrator
 */
#include "variabledef.h"
#include "accSFloat2.h"
#include "accLSFloat.h"

#ifndef ACCSINT_H_
#define ACCSINT_H_

class CaccSint
{
public:
	enum STATUS status;

	struct IN
	{
		uint16_t pauseCmd;
		long length;
		float setV;
		float setA;
		float setJ;
		unsigned int valid;
	} in;

	struct VIEW
	{
		long position;
		float v;
		float a;
		unsigned int valid;
	} view;

private:
	CaccSFloat2 obj;
	// CaccLSFloat obj;
	uint32_t ullength;
	int dir;
	long bakPosition;
	float t;
	float deltaT;
	float Vmax;
	float Amax;
	float J;

public:
	CaccSint() { in.valid = 0; }
	void reset(void);
	void sureInput(void);
	void run(void);

}; // endof call define

#endif /* ACCSINT_H_ */
