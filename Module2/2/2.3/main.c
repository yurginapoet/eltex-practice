#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calc.h"

#define MAX_NUMS 5
#define BUF_SIZE 256

int parse_input(double *arr) {
    char buf[BUF_SIZE];
    if (!fgets(buf, BUF_SIZE, stdin)) return -1;

    int count = 0;
    char *token = strtok(buf, " \n\t");

    while (token && count < MAX_NUMS) {
        char *endptr;
        double value = strtod(token, &endptr);

        if (token == endptr || *endptr != '\0') return -1;

        arr[count++] = value;
        token = strtok(NULL, " \n\t");
    }
    return count;
}

int main() {
    int choice;
    double nums[MAX_NUMS];

    while (1) {
        printf("\nAvailable operations (list generated dynamically):\n");
        for (int i = 0; i < num_operations; i++) 
            printf("%d: %s (%s)\n", operations[i].op, 
                   operations[i].symbol, operations[i].description);
                   
        printf("0: exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Error: input is not a number\n");
            while (getchar() != '\n');
            continue;
        }

        if (choice == 0) break;

        char symbol[4] = "";
        for (int i = 0; i < num_operations; i++) {
            if (operations[i].op == choice) {
                strcpy(symbol, operations[i].symbol);
                break;
            }
        }

        if (strlen(symbol) == 0) {
            printf("Error: unknown operation\n");
            while (getchar() != '\n');
            continue;
        }

        printf("Enter numbers (up to %d, separated by spaces): ", MAX_NUMS);
        while (getchar() != '\n');

        int n = parse_input(nums);

        if (n == -1) {
            printf("Error: all arguments must be numbers\n");
            continue;
        }
        if (n == 0) {
            printf("Error: no numbers entered\n");
            continue;
        }
        if (n > 5) {
            printf("Error: too many arguments (maximum 5)\n");
            continue;
        }

        if (choice == 4 || choice == 6) {
            int error = 0;
            for (int i = 1; i < n; i++) 
                if (nums[i] == 0.0) {
                    error = 1;
                    break;
                }
            if (error) {
                printf("Error: division by zero\n");
                continue;
            }
        }

        double res = call_operation(choice, n, nums);
        printf("Result %s: %.10g\n", symbol, res);
    }

    printf("Program terminated.\n");
    return 0;
}
