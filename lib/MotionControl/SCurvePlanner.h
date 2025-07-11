#ifndef __SCURVEPLANNER_H__
#define __SCURVEPLANNER_H__

#include "ScurvePlan.h"

class SCurvePlanner {
public:
    SCurvePlanner() {}
    ~SCurvePlanner() {}
    int Plan(double l, double vs, double vm, double ve, double j, double nm, double ts);
    double Execute();
    bool IsExecuteOk();
    double GetSpeed();
    double GetPosition();
    int32_t GetOutput();
private:
    SCurveParams sp;
};



#endif
