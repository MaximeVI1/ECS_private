#ifndef MATH_CALCULUS_POLYNOMIAL_H
#define MATH_CALCULUS_POLYNOMIAL_H

#include <math.h>

#include <math_rob_c/numbers/numbers.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct quadratic_equation_s{
    double c2, c1, c0;  ///< c2*x^2 + c1*x + c0 = 0
    double delta;  ///< delta = c1^2 - 4c2c0
    complex_number_t roots[2];  
}quadratic_equation_t;

/**
 * @brief Compute the (two) roots of a quadratic polynomial
 * 
 * @param polynomial 
 */
void find_roots_quadratic_equation(quadratic_equation_t *polynomial);

#ifdef __cplusplus
}
#endif

#endif
