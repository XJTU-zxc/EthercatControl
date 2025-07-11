#include "SCurvePlanner.h"


int SCurvePlanner::Plan(double l, double vs, double vm, double ve, double j, double nm, double ts)
{
    int rc = SCurvePlan(&this->sp, l, vs, vm, ve, j, nm, ts);
    return rc;
}

double SCurvePlanner::Execute()
{
    return SCurveExecute(&this->sp);
}

bool SCurvePlanner::IsExecuteOk()
{
    return this->sp.so.isOK == 1;
}

double SCurvePlanner::GetSpeed()
{
    return this->sp.so.vreal;
}

double SCurvePlanner::GetPosition()
{
    return this->sp.so.sreal;
}

int32_t SCurvePlanner::GetOutput()
{
    return (int32_t)this->sp.so.sreal - (int32_t)this->sp.so.last_sreal;
}