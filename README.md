# ECE 528/L - Robotics and Embedded Systems Lab
**CSU Northridge**

**Department of Electrical and Computer Engineering**

## Overview

Title: Lab 3 - Line Follower

Author: Dominic Sanqui

Date: 10/23/26

Description: The purpose of this lab is to use the concepts from lab 2 about how to configure the QTRX Reflectance Sensor Array to read data and use it to move the TI-RSLK MAX robot. The robot will be placed in a maze of white paper and lines of black tape. The robot must follow the lines of black tape to move from the beginning of the maze to the end and back to the beginning all by itself.

## Components Used
- 1 MSP432 Launchpad
- 1 TI-RSLK MAX
- 1 8-Channel QTRX Reflectance Sensor Array
- 1 USB-A to Micro-USB Cable
- User LEDs of the TI MSP432 LaunchPad
- Several Pages of White Paper
- Several strips of black tape

## Prelab

1. Write a void function named Chassis_Board_LEDs_Init that takes no arguments and initializes the following pins as output GPIO pins. Initilaize the value of the pins to zero.

```
void Chassis_Board_LEDs_Init(void)
{
    P8->SEL0 &= ~0xE1;
    P8->SEL1 &= ~0xE1;
    P8->OUT &= ~0xE1;
    P8->DIR |= 0xE1;3
}
```

2. Write a void function named Print_Tenth that takes no arguments. Your function must iterate from 10 to 51 using a for-loop. Print a message for every 10th iteration (10, 20, 30, 40, 50) that states "Index %d: Every 10h iteration.\n" and then print a different message for the iterations that follow after the 10th iteration (11, 21, 31, 41, 51) that states "Index %d: Following the 10th iteration.\n".

```
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
```

When the Print_Tenth() function is called, it iterates through a for loop. Based on the current iteration of the for loop, it generates a message. Figure 1 shows the output of the function.

Figure 1: Print_Tenth.c Prelab Test Cases

![Figure 1](/Screenshots/ece528L_lab3_prelab_print_tenth.png)

## Analysis and Results

This lab reuses several portions of code from lab 2 such as the enumeration list that displays the different positions that the robot can be in and the Detect_Line_Position function that is used to provide instructions for the robot to perform based on what position it is in. However, the Detect_Line_Position function has been drastically modified for this lab. Before, this function only contained instructions to change the color of the LEDs based on the position that the robot is in. Now, this function also includes instructions on how the robot should move based on the position the robot is in by calling the motor functions that were generated back in Lab 1. The duty cycle provided to the motor functions were generated based on observations from testing the robot as it moved between the maze. For example when the robot is turning, the speed of one motor is greater than the other motor to provide the arc that the robot needs to turn.

To call the Detect_Line_Position function, interrupts are used instead of a while loop in the main function. These interrupts can come from the SysTick and Timer_A1 modules. For this lab, the Timer_A1 interrupts will be discussed only. The Timer_A1 module continuously runs in the background. After 1 ms has passed, the Timer_A1 will trigger the interrupt.

When the interrupt occurs, several things happen. The Launchpad will begin keeping track of the number of times the interrupt occurs. After the 10th interrupt occurs, the Launchpad will begin reading the sensor data to determine the position of the robot. After the 11th interrupt occurs, the Launchpad will finish reading the sensor data. Then, it will check the status of several global variables to determine how the robot should react.

If the robot is moving in a line, it is easy for it to interpret its position. If the robot needs to make a turn, it is a lot more difficult. This is due to the fact that the turning of the robot will drastically change its position which will lead to confusion. If its in the middle of the making a turn, it may see that it is centered and move in a straight line off of the maze. Additionally, it has been noted that the robot may not have enough time to acquire the correct position data from the sensor. The sudden change in the location of the black tape strips can cause the robot to believe it is at a turn instead of the T-intersection and perform the wrong action for example.

To counter this issue, several global variables were created. The variable uint8_global_turn_status is used to indicate if the robot is in the middle of a turn or intersection. If it is 0, the robot will continue to move along in a straight line. If it is 1, the robot will perform the turn until it receives the sensor data that indicates it is now on the center line. The robot will then clear this variable and return to the logic that it should move in a line. This ensures that there is no confusion between turning the robot and being off center as it would immediately switch states from turning to something else. 

The second global variable is uint8_global_turn_count. This variable ensures that the robot has enough time to interpret that it should be performing a turn. From testing, the robot is not able to reacquire the line if it attempts to perform a turn too early which results in an indefinite turn loop. This variable ensures that the robot is in the correct position to perform the turn by forcing the robot to wait before the turn. If the sensor data reads that the there is a upcoming turn after several interrupts have passed, the robot will then perform the turn and this variable will be cleared.

The last global variable is the enum_turn_direction. This variable saves the turn direction that the robot should perform when it reaches the turn or intersection in the maze. Once the uint8_global_turn_status variable has been set to 1 to tell the robot to begin turning, the enum_turn_direction will be continuously fed to the Detect_Line_Position. This will force the robot to continue the turn while preventing it by being overrun by the continuous position updates from the sensor. Once the sensor detects it has reached the center of the line, this variable is cleared and the robot will continue using the data received directly from the sensor.

With the code complete, the testing of the robot in the maze can be peformed. There are no screenshots to present, but it will be presented to the professor in the lab. The results show that the robot was able to successfully navigate through the maze.

## Known Issues or Limitations

It's difficult to consistently get the robot from one end of the maze to the other. From testing, the robot makes it through the maze 40% of the time. The main issues are the turns and the T-intersection. The robot has difficulty in determining whether it is has reached a turn or an intersection. If the robot is performing either of these movements, the robot may unintentionally move in a line as the continuous position updates from the sensor may override the turn or intersection. There is no reliable solution to fix this issue, so the testing is mainly trial and error.

## References
* Technical Reference Manual for TI MSP432 Launchpad
[Product Link](https://web.archive.org/web/20200402132841/http:/www.ti.com/lit/ug/slau356i/slau356i.pdf)
* User LEDs of the TI MSP432 LaunchPad
* Pololu Gearmotor with Encoder - [Product Link](https://www.pololu.com/product/3675)
* 8-Channel QTRX Sensor Array - [Product Link](https://www.pololu.com/product/3672)




