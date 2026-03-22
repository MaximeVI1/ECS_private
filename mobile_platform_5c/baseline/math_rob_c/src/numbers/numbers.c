#include <math_rob_c/numbers/numbers.h>
#include <stdlib.h>

double max(double a, double b){
    return (a > b) ? a : b;
}

double min(double a, double b){
    return (a < b) ? a : b;
}

int max_int(int a, int b){
    return (a > b) ? a : b;
}

int min_int(int a, int b){
    return (a < b) ? a : b;
}

int compare_double (const void * a, const void * b)
{
  if ( *(double*) a <  *(double*) b ) return -1;
  if ( *(double*) a >  *(double*) b ) return 1;
  if ( *(double*) a == *(double*) b ) return 0;
}

double smallest_value(const double *values, int number_of_values, int *index){

    int index_local = -1;
    if (index != NULL){
        *index = index_local;
    }else{
        index = &index_local;
    }

    double smallest_value = INFINITY;
    for(int i=0; i < number_of_values; i++){
        if(values[i] < INFINITY){
            if (values[i] < smallest_value){
                smallest_value = values[i];
                *index = i;
            }
        }
    }

    return smallest_value;      
}

double mean_value(const double *values, int number_of_values){
    
    double sum =0;
    for(int i=0; i < number_of_values; i++){
        sum += values[i];
    }

    if(number_of_values > 0){
        return sum / number_of_values;
    }else{
        return INFINITY;
    }
}

double wrap_to_pi(double angle) {
    double res;
    if (angle > M_PI) {
        res = angle - 2*M_PI;
    } else if (angle < -M_PI) {
        res = angle + 2*M_PI;
    } else {
        res = angle;
    }
    return res;
}
