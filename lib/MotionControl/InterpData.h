#ifndef __INTERPDATA_H__
#define __INTERPDATA_H__

#include <stdint.h>

#include "pmcsystem.h"



enum {
    INTERP_TYPE_LINE = 0,
    INTERP_TYPE_CIRCLE = 1
};


struct InterpCircleData {
    pmc_real_t vm;
    pmc_real_t am;
    int circle_type;
    int32_t edx;
    int32_t edy;
    int32_t i;
    int32_t j;
    int32_t r;
    int32_t q;
    int32_t d;
};

struct InterpLineData {
    pmc_real_t vs;
    pmc_real_t vm;
    pmc_real_t ve;
    pmc_real_t am;
    pmc_real_t dist;
    pmc_real_t unitVec[8];
    // pmc_real_t j;
    // pmc_real_t nm;
    // pmc_real_t ts;
};

struct InterpData {
    static pmc_real_t MPE() {return 0.001;}

//    pmc_real_t vs;
//    pmc_real_t vm;
//    pmc_real_t ve;
//    pmc_real_t dist;
//    pmc_real_t unitVec[8];
    // pmc_real_t j;
    // pmc_real_t nm;
    // pmc_real_t ts;
    int type;
    union {
        InterpLineData lineData;
        InterpCircleData circleData;
    } data;

};




#endif
