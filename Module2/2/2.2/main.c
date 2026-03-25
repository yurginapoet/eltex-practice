#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calc.h"

#define MAX_NUMS 10
#define BUF_SIZE 256

int parse_input(double *arr) {
    char buf[BUF_SIZE];
    if (!fgets(buf, BUF_SIZE, stdin)) return -1;

    int count = 0;
    char *token = strtok(buf, " \n");

    while (token && count < MAX_NUMS) {
        char *endptr;
        double value = strtod(token, &endptr);

        if (*token == '\0' || *endptr != '\0') return -1;

        arr[count++] = value;
        token = strtok(NULL, " \n");
    }

    return count;
}

double call_func(int op, int n, double *a) {
    CalcFunc func = NULL;

    switch (op) {
        case 1:
            func = add;
            break;
        case 2:
            func = sub;
            break;
        case 3:
            func = mul;
            break;
        case 4:
            func = divide;
            break;
        default:
            printf("Unknown operation\n");
            return 0;
    }

    switch (n) {
        case 1:
            return func(1, a[0]);
        case 2:
            return func(2, a[0], a[1]);
        case 3:
            return func(3, a[0], a[1], a[2]);
        case 4:
            return func(4, a[0], a[1], a[2], a[3]);
        case 5:
            return func(5, a[0], a[1], a[2], a[3], a[4]);
        default:
            printf("Too many args (max 5)\n");
            return 0;
    }
}

int main() {
    int op;
    double nums[MAX_NUMS];

    while (1) {
        printf("1: + \n2: - \n3: * \n4: / \n0: exit\n");
        printf("Choice: ");

        if (scanf("%d", &op) != 1) {
            printf("Invalid input (not a number)\n");
            while (getchar() != '\n');
            continue;
        }

        if (op < 0 || op > 4) {
            printf("Invalid choice (0-4 only)\n");
            continue;
        }

        getchar(); 

        if (op == 0) break;

        printf("Enter numbers: ");
        int n = parse_input(nums);

        if (n == -1) {
            printf("Error: all arguments must be numbers\n");
            continue;
        }

        if (n == 0) {
            printf("Error: no arguments provided\n");
            continue;
        }
        
        if (op == 4) {
            int error = 0;
        
            for (int i = 1; i < n; i++) 
                if (nums[i] == 0) {
                    error = 1;
                    break;
                }

            if (error) {
                printf("Error: division by zero\n");
                continue;
            }
        }
        double res = call_func(op, n, nums);
        
        printf("Result: %lf\n", res);
    }
}
