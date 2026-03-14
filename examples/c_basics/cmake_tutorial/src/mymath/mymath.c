#include <mymath/mymath.h>

void wrap_angle_to_180(double input, double *output)
{
    if (input > 180){
        *output = input - 360;
    }else if (input < -180)
    {
        *output = input + 360;
    }
}
