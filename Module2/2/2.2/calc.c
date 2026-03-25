#include <stdarg.h>
#include "calc.h"

double add(int n, ...) {
    va_list args;
    va_start(args, n);

    double res = 0;
    for (int i = 0; i < n; i++) 
        res += va_arg(args, double);

    va_end(args);
    return res;
}

double sub(int n, ...) {
    va_list args;
    va_start(args, n);

    double res = va_arg(args, double);
    for (int i = 1; i < n; i++) 
        res -= va_arg(args, double);

    va_end(args);
    return res;
}

double mul(int n, ...) {
    va_list args;
    va_start(args, n);

    double res = 1;
    for (int i = 0; i < n; i++) 
        res *= va_arg(args, double);

    va_end(args);
    return res;
}

double divide(int n, ...) {
    va_list args;
    va_start(args, n);

    double res = va_arg(args, double);
    for (int i = 1; i < n; i++) 
        res /= va_arg(args, double);

    va_end(args);
    return res;
}