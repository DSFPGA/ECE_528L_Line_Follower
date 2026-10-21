/**
 * @file print_tenth.c
 * 
 * @brief Main source code for Print_Tenth program
 * 
 * This file contains the main entry point and the implementation
 * of the Print_Tenth function that iterates from 10 to 51 and 
 * prints a message every 10th iteration (10, 20, 30, 40, 50)
 * and a different message following the 10th iteration (11, 21,
 * 31, 41, 51)
 * 
 * @author Dominic Sanqui
 * 
 * */

#include <stdio.h>

void Print_Tenth();

int main()
{
    Print_Tenth();
    return 0;
}

void Print_Tenth() 
{
    for (int i = 10; i < 52; i++) {
        switch(i) 
        {
            case 10: 
            {
                printf("Index %d: Every 10th iteration.\n", i);
            }
            continue;

            case 20: 
            {
                printf("Index %d: Every 10th iteration.\n", i);
            }
            continue;

            case 30: 
            {
                printf("Index %d: Every 10th iteration.\n", i);
            }
            continue;

            case 40: 
            {
                printf("Index %d: Every 10th iteration.\n", i);
            }
            continue;

            case 50: 
            {
                printf("Index %d: Every 10th iteration.\n", i);
            }
            continue;

            case 11: 
            {
                printf("Index %d: Following the 10th iteration.\n", i);
            }
            continue;

            case 21: 
            {
                printf("Index %d: Following the 10th iteration.\n", i);
            }
            continue;

            case 31: 
            {
                printf("Index %d: Following the 10th iteration.\n", i);
            }
            continue;

            case 41: 
            {
                printf("Index %d: Following the 10th iteration.\n", i);
            }
            continue;

            case 51: 
            {
                printf("Index %d: Following the 10th iteration.\n", i);
            }
            continue;

            default: 
            continue;
        }
    }
}