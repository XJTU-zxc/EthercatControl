/*
 * accMulAxis.h
 *
 *  Created on: 2014-11-28
 *      Author: zzk
 */
#include "variabledef.h"
#include "accSFloat2.h"


#ifndef accMulAxis_H_
#define accMulAxis_H_

class CaccMulAxis
{
public:
	enum STATUS status;

	struct IN
	{	uint16_t		pauseCmd;
		long   		lengthX;
		long   		lengthY;
		long   		lengthZ;
		long   		lengthA;
		long   		lengthB;
		long   		lengthC;
        long        lengthW;
        long        lengthU;
		float		setV;
		float		setA;
		float		setJ;
		unsigned int 	valid;
	} in;


	struct VIEW
	{	long			pos_x;
		long			pos_y;
		long			pos_z;
		long			pos_a;
		long			pos_b;
		long			pos_c;
        long            pos_w;
        long            pos_u;
		float			v;
		unsigned int	valid;
	} view;

private:

	CaccSFloat2   obj;


 	float   kx;
 	float   ky;
 	float   kz;
 	float   ka;
 	float   kb;
 	float   kc;
    float   kw;
    float   ku;
 	long    bakX;
 	long    bakY;
 	long    bakZ;
 	long    bakA;
 	long    bakB;
 	long    bakC;
    long    bakW;
    long    bakU;
 	int  dirX;
 	int  dirY;
 	int  dirZ;
 	int  dirA;
 	int  dirB;
 	int  dirC;
    int  dirW;
    int  dirU;


//	float	t;
//	float   deltaT;
//	float   Vmax;
//	float   Amax;
//	float   J;
public:
	CaccMulAxis(){ in.valid=0; }
	void reset(void);
	void sureInput(void);
	void run(void);

};//endof call define










#endif /* ACCMULAXIS_H_ */
