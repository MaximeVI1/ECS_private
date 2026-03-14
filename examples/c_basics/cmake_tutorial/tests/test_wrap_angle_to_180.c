#include <mymath/mymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    if (argc != 2){
        printf("wrong number of arguments\n");
        return -1;
    }

    double input = atof(argv[1]);
    double output;
    wrap_angle_to_180(input, &output);
    assert(output<=180 && output>=-180);
    printf("input angle: %f\n", input);
    printf("output angle: %f\n", output);

    return 0;   
}