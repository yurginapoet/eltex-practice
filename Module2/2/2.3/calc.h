#ifndef CALC_H
#define CALC_H

typedef double (*CalcFunc)(int n, ...);

typedef struct {
    int op;
    char symbol[4];
    char description[32];
    CalcFunc func;
} Operation;

extern Operation operations[];
extern int num_operations;

double add(int n, ...);
double sub(int n, ...);
double mul(int n, ...);
double divide(int n, ...);
double calc_pow(int n, ...);
double calc_mod(int n, ...);
double calc_min(int n, ...);
double calc_max(int n, ...);
double calc_avg(int n, ...);

double call_operation(int op, int n, double *args);

#endif