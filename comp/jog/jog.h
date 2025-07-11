/*
 * jog.h
 *
 *  Created on: 2014-12-10
 *      Author: Administrator
 */
#include "variabledef.h"
//#include "comp/accSFloat2/accSFloat2.h"


#ifndef JOG_H_
#define JOG_H_


class Cjog
{
public:
    enum STATUS status;

    struct IN
    {   float       setV;
        float       setAcc;
        float       setJ;
        int         dir;//
        uint16_t      cmdHiBrake;
        uint16_t      HiBrakeValue;
        unsigned int    valid;
    } in;


    struct VIEW
    {   int             delta;
        unsigned int    valid;
    } view;

private:
    long  posiBak;
    float a,v,t;
    float f_posi;
    float V01,V02;
    float T1,T2;
    uint16_t phaseBrake;



public:
    Cjog(){ in.valid=0; }
    void reset(void);
    void sureInput(void);
    void runUp(void);
    void runDown(void);
    void calBrake(void);
};//endof call define



#endif /* JOG_H_ */
