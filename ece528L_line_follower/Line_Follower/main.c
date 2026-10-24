/**
 * @file main.c
 *
 * @brief Main source code for the Line_Follower program.
 *
 * This file contains the main entry point for the Line_Follower program.
 * The main controller demonstrates a Line Follower robot without using an advanced algorithm.
 *
 * It interfaces the following peripherals using GPIO to demonstrate line following:
 *  - 8-Channel QTRX Sensor Array module
 *
 * Timers are used in this lab:
 *  - SysTick:  Used to generate periodic interrupts at a specified rate (1 kHz)
 *  - Timer A0: Used to generate PWM signals that will be used to drive the DC motors
 *  - Timer A1: Used to generate periodic interrupts at a specified rate (1 kHz)
 *
 * @note For more information regarding the 8-Channel QTRX Sensor Array module,
 * refer to the product page: https://www.pololu.com/product/3672
 *
 * @author
 *
 */

#include "msp.h"
#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/Timer_A1_Interrupt.h"
#include "inc/SysTick_Interrupt.h"
#include "inc/Reflectance_Sensor.h"

// Initialize constant PWM duty cycle values for the motors
#define PWM_NOMINAL 2500
#define PWM_FAST 2000
#define PWM_SLOW 1500
#define PWM_SUPER_SLOW 1000

// Initialize a global variable for Timer A1 to keep track of elapsed time in milliseconds
static uint32_t Timer_A1_ms_elapsed = 0;

typedef enum {
    CENTER = 0x18,
    SLIGHTLY_LEFT = 0x1C,
    SLIGHTLY_RIGHT = 0x38,
    FAR_LEFT = 0x01,
    FAR_RIGHT = 0x80,
    LEFT_TURN = 0xF8,
    RIGHT_TURN = 0x1F,
    T_INTERSECTION = 0xFF,
    DEAD_END = 0x00,
    CHECK_FOR_TURN = 0xAA
} enum_Line_Position;

// Create global variables to check if robot is in the process of turning
enum_Line_Position enum_turn_direction = DEAD_END;
uint8_t uint8_global_turn_status = 0;
uint8_t uint8_global_turn_count = 0;

/**
 * @brief Set the color of the LEDs based on the value of the reflectance sensor
 *
 * The reflectance sensor underneath the robot will generate a 8 bit integer value that represents the position
 * of the robot within the maze. If a bit is 1, then it detects the black tape. If a bit is 0, then it detects the white paper.
 * Based on the reading from the sensor, the robot will set the RGB LED to a specific color to indicate what position it is in
 *
 * @param reflactance_sensor_data - Value of the sensor
 *
 * @return None
 */

void Detect_Line_Position(uint8_t reflectance_sensor_data) {

    switch(reflectance_sensor_data) {
    case CENTER:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_GREEN);
            //Motor_Stop();
            //Clock_Delay1us(100);
            Motor_Forward(PWM_SLOW, PWM_SLOW);
        }
        break;

    case SLIGHTLY_LEFT:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_YELLOW);
            Motor_Stop();
            Clock_Delay1us(500);
            Motor_Forward(PWM_SLOW, PWM_SUPER_SLOW);
        }
        break;

    case SLIGHTLY_RIGHT:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_PINK);
            Motor_Stop();
            Clock_Delay1us(500);
            Motor_Forward(PWM_SUPER_SLOW, PWM_SLOW);
        }
        break;

    case FAR_LEFT:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_WHITE);
            Motor_Stop();
            Clock_Delay1us(500);
            Motor_Forward(PWM_SLOW, PWM_FAST);
        }
        break;

    case FAR_RIGHT:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_SKY_BLUE);
            Motor_Stop();
            Clock_Delay1us(500);
            Motor_Forward(PWM_FAST, PWM_SLOW);
        }
        break;

    case LEFT_TURN:
        {
            LED1_Output(RED_LED_ON);
            LED2_Output(RGB_LED_WHITE);
            Motor_Stop();
            Clock_Delay1us(500);
            Motor_Left(PWM_SUPER_SLOW, PWM_SLOW);
            Clock_Delay1us(500);
        }
        break;

    case RIGHT_TURN:
        {
            LED1_Output(RED_LED_ON);
            LED2_Output(RGB_LED_SKY_BLUE);
            Motor_Stop();
            Clock_Delay1us(500);
            Motor_Right(PWM_SLOW, PWM_SUPER_SLOW);
            Clock_Delay1us(500);
        }
        break;

    case T_INTERSECTION:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_BLUE);
            Motor_Left(PWM_FAST, PWM_FAST);
            Clock_Delay1us(500);
        }
        break;

    case DEAD_END:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_RED);
            //Motor_Stop();
            //Clock_Delay1us(100);
            //Motor_Backward(PWM_SLOW, PWM_SLOW);
            Motor_Stop();
        }
        break;

    case CHECK_FOR_TURN:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_OFF);
            Motor_Stop();
            Clock_Delay1us(500);
        }

    default:
        {
            LED1_Output(RED_LED_OFF);
            LED2_Output(RGB_LED_OFF);
            Motor_Forward(PWM_SUPER_SLOW, PWM_SUPER_SLOW);
            Clock_Delay1us(500);
        }
        break;
    }
}

void Timer_A1_Periodic_Task(void)
{
    //Increment Timer_A1_ms_elapsed by 1 every time the Timer A1 periodic interrupt occurs
    Timer_A1_ms_elapsed++;

    // Start the process of reading the reflectance sensor array every 10 ms (i.e. 10, 20, 30...)
    if ((Timer_A1_ms_elapsed % 10) == 0)
    {
        Reflectance_Sensor_Start();
    }

    // Finish reading the reflectance sensor array after 1 ms (i.e. 11, 21, 31, ...)
    if ((Timer_A1_ms_elapsed % 10) == 1)
    {
        uint8_t Reflectance_Sensor_Data = Reflectance_Sensor_End();
        Motor_Stop();

        if (uint8_global_turn_status == 0)
        {

            if (Reflectance_Sensor_Data == LEFT_TURN)
            {
                Detect_Line_Position(CHECK_FOR_TURN);
                uint8_global_turn_count++;
                enum_turn_direction = LEFT_TURN;
            }

            else if (Reflectance_Sensor_Data == RIGHT_TURN)
            {
                Detect_Line_Position(CHECK_FOR_TURN);
                uint8_global_turn_count++;
                enum_turn_direction = RIGHT_TURN;
            }

            else if (Reflectance_Sensor_Data == T_INTERSECTION)
            {
                Detect_Line_Position(CHECK_FOR_TURN);
                uint8_global_turn_count++;
                enum_turn_direction = T_INTERSECTION;
            }
            else
            {
                Detect_Line_Position(Reflectance_Sensor_Data);
                uint8_global_turn_count = 0;
            }

            if (uint8_global_turn_count == 15)
            {
                uint8_global_turn_status = 1;
                uint8_global_turn_count = 0;
            }
            //printf("uint8_global_turn_status = %d, uint8_global_turn_count = %d, Reflectance_Sensor_Data = %02X, enum_turn_direction = %02X\n", uint8_global_turn_status, uint8_global_turn_count, Reflectance_Sensor_Data, enum_turn_direction);
        }

        else
        {
            Detect_Line_Position(enum_turn_direction);

            if (Reflectance_Sensor_Data == CENTER)
            {
                uint8_global_turn_status = 0;
                enum_turn_direction = DEAD_END;
            }
        }

    }
}

int main(void)
{
    //Ensure the interrupts are disabled during initalization
    DisableInterrupts();

    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the built-in red LED and the RGB LED on the MSP432 microcontroller
    LED1_Init();
    LED2_Init();

    // Initialize the motors
    Motor_Init(PWM_NOMINAL);

    // Initialize the EUSCI_A0_UART module
    EUSCI_A0_UART_Init_Printf();

    // Initialize the 8-Channel QTRX Reflectance Sensor Array Module
    Reflectance_Sensor_Init();

    // Initialize the SysTick timer to generate periodic interrupts every 1 ms
    SysTick_Interrupt_Init(SYSTICK_INT_NUM_CLK_CYCLES, SYSTICK_INT_PRIORITY);

    // Initialize Timer A1 periodic interrups every 1 ms
    Timer_A1_Interrupt_Init(&Timer_A1_Periodic_Task, TIMER_A1_INT_CCR0_VALUE);

    // Enable the interrupts used by the modules
    EnableInterrupts();

    while(1)
    {

    }
}

/*
void Chassis_Board_LEDs_Init(void)
{
    P8->SEL0 &= ~0xE1;
    P8->SEL1 &= ~0xE1;
    P8->OUT &= ~0xE1;
    P8->DIR |= 0xE1;3
}
*/
