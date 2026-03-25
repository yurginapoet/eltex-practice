#include <stdarg.h>
#include <math.h>
// #include <stddef.h>
#include "calc.h"

double add(int n, ...) {
    va_list args;
    va_start(args, n);
    double res = 0.0;
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
    double res = 1.0;
    for (int i = 0; i < n; i++) 
        res *= va_arg(args, double);
    va_end(args);
    return res;
}

double divide(int n, ...) {
    va_list args;
    va_start(args, n);
    double res = va_arg(args, double);
    for (int i = 1; i < n; i++) {
        double d = va_arg(args, double);
        if (d == 0.0) {
            va_end(args);
            return NAN;
        }
    }
    va_end(args);
    va_start(args, n);
    res = va_arg(args, double);
    for (int i = 1; i < n; i++) 
        res /= va_arg(args, double);
    va_end(args);
    return res;
}

double calc_pow(int n, ...) {
    va_list args;
    va_start(args, n);
    if (n == 0) {
        va_end(args);
        return 0.0;
    }
    double res = va_arg(args, double);
    for (int i = 1; i < n; i++) {
        double exp = va_arg(args, double);
        res = pow(res, exp);
    }
    va_end(args);
    return res;
}

double calc_mod(int n, ...) {
    va_list args;
    va_start(args, n);
    if (n < 2) {
        va_end(args);
        return 0.0;
    }
    double res = va_arg(args, double);
    for (int i = 1; i < n; i++) {
        double d = va_arg(args, double);
        if (d == 0.0) {
            va_end(args);
            return NAN;
        }
        res = fmod(res, d);
    }
    va_end(args);
    return res;
}

double calc_min(int n, ...) {
    va_list args;
    va_start(args, n);
    if (n == 0) {
        va_end(args);
        return 0.0;
    }
    double res = va_arg(args, double);
    for (int i = 1; i < n; i++) {
        double v = va_arg(args, double);
        if (v < res) res = v;
    }
    va_end(args);
    return res;
}

double calc_max(int n, ...) {
    va_list args;
    va_start(args, n);
    if (n == 0) {
        va_end(args);
        return 0.0;
    }
    double res = va_arg(args, double);
    for (int i = 1; i < n; i++) {
        double v = va_arg(args, double);
        if (v > res) res = v;
    }
    va_end(args);
    return res;
}

double calc_avg(int n, ...) {
    va_list args;
    va_start(args, n);
    if (n == 0) {
        va_end(args);
        return 0.0;
    }
    double sum = 0.0;
    for (int i = 0; i < n; i++) 
        sum += va_arg(args, double);
    va_end(args);
    return sum / n;
}

double call_operation(int op, int n, double *args) {
    CalcFunc func = NULL;
    for (int i = 0; i < num_operations; i++) {
        if (operations[i].op == op) {
            func = operations[i].func;
            break;
        }
    }
    if (func == NULL || n < 1 || n > 5) {
        return 0.0;
    }
    switch (n) {
        case 1: return func(1, args[0]);
        case 2: return func(2, args[0], args[1]);
        case 3: return func(3, args[0], args[1], args[2]);
        case 4: return func(4, args[0], args[1], args[2], args[3]);
        case 5: return func(5, args[0], args[1], args[2], args[3], args[4]);
    }
    return 0.0;
}

Operation operations[] = {
    {1, "+",  "Addition",                     add},
    {2, "-",  "Subtraction",                  sub},
    {3, "*",  "Multiplication",               mul},
    {4, "/",  "Division",                     divide},
    {5, "^",  "Exponentiation",               calc_pow},
    {6, "%",  "Modulo",                       calc_mod},
    {7, "min","Minimum",                      calc_min},
    {8, "max","Maximum",                      calc_max},
    {9, "avg","Average",                      calc_avg}
};

int num_operations = sizeof(operations) / sizeof(operations[0]);
