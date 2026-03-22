#ifndef MATH_ROB_C_NUMBERS_H
#define MATH_ROB_C_NUMBERS_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct complex_number_s{
    double real, imaginary;
}complex_number_t;

/**
 * @brief Given two values (double), returns the largest one.
 * 
 * @param a 
 * @param b 
 * @return double 
 */
double max(double a, double b);

/**
 * @brief Given two values (double), returns the smallest one.
 * 
 * @param a 
 * @param b 
 * @return double 
 */
double min(double a, double b);

/**
 * @brief Given two values (int), returns the largest one.
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int max_int(int a, int b);

/**
 * @brief Given two values (int), returns the smallest one.
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int min_int(int a, int b);

/**
 * @brief Compare function suitable for stdlib's qsort when
 * values to be sorted are of type double.
 * 
 * @param a 
 * @param b 
 * @return int positive if a is greater than b, negative otherwise.
 */
int compare_double (const void * a, const void * b);

/**
 * Returns the minimum value of an array of doubles. Values equal
 * to the INFINITY constant are discarded. If a non-NULL pointer
 * is passed in the index argument, it provides the index of the 
 * smallest value in the input array. 
 * 
 * @param values 
 * @param number_of_values 
 * @param index index of smallest_value in array values
 * @return double result is valid if smaller than INFINITY
 */
double smallest_value(const double *values, int number_of_values, int *index);

/**
 * @brief Computes the mean of an array of doubles
 * 
 * @param values 
 * @param number_of_values 
 * @return double 
 */
double mean_value(const double *values, int number_of_values);

/**
 * @brief Wrap an angle to the interval [-pi, pi].
 * 
 * @param angle in radians
 * @return double 
 */
double wrap_to_pi(double angle);



#ifdef __cplusplus
}
#endif

#endif  // MATH_NUMBERS_H