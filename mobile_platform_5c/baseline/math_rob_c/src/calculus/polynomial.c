#include <math_rob_c/calculus/polynomial.h>

void find_roots_quadratic_equation(quadratic_equation_t *polynomial){
    double a = polynomial->c2;
    double b = polynomial->c1;
    double c = polynomial->c0;
    
    polynomial->delta = pow(b, 2) - 4*a*c;
    if (polynomial->delta >= 0){
        polynomial->roots[0].real = (-b - sqrt(polynomial->delta))/(2*a);
        polynomial->roots[1].real = (-b + sqrt(polynomial->delta))/(2*a);
        polynomial->roots[0].imaginary = 0;
        polynomial->roots[1].imaginary = 0;
    } else{
        polynomial->roots[0].real = -b/(2*a);
        polynomial->roots[1].real = polynomial->roots[0].real;
        polynomial->roots[0].imaginary = sqrt(-polynomial->delta)/(2*a);
        polynomial->roots[1].imaginary = -polynomial->roots[0].imaginary;
    }
}
