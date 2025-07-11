//#include "math.h"
#ifndef accLineFloat_H
#define accLineFloat_H

#include "variabledef.h"
//#ifdef __cplusplus
//extern "C" {
//#endif
//-------------------------------------------

class CaccLineFloat
{
public:

	enum STATUS status;

	struct IN
	{	long   			length;
		unsigned int 	valid;
	} in;

	struct VIEW
	{
		//long			position;
		float 			position;
		float			v;
		unsigned int	valid;
	} view;

private:
 	unsigned int phase;
	int remain;

	float 	distance;
	float   T1;
	float	T2;
	float   S1max;
	float   T1max;
	float	t;
	float   deltaT;
	float   Vmax;
	float   A;

public:
	void reset(void);
	void sureInput(void); 	//{ remain=in.length; in.valid=1; }
	void run(void);
};
//------------------------------------------
#endif

