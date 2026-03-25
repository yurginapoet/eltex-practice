#ifndef CALC_H
#define CALC_H

typedef double (*CalcFunc)(int n, ...);

double add(int n, ...);
double sub(int n, ...);
double mul(int n, ...);
double divide(int n, ...);

#endif