
#include "steppermotors.h"
#include "stdlib.h"

void Stepper_Step(int step) {
    switch(step) {
        case 0:
            HAL_GPIO_WritePin(MOTOR_PORT, IN1, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN2, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN3, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN4, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(MOTOR_PORT, IN1, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN2, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN3, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN4, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(MOTOR_PORT, IN1, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN2, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN3, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN4, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(MOTOR_PORT, IN1, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN2, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN3, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_PORT, IN4, GPIO_PIN_SET);
            break;
    }
}


void Stepper_Move(int steps, int delay) {
    int direction = (steps > 0) ? 1 : -1;  // Determine direction
    int stepCount = abs(steps);            // Get absolute step count

    for (int i = 0; i < stepCount; i++) {
        int stepIndex = (direction > 0) ? (i % 4) : ((3 - (i % 4)));  // Reverse sequence for negative steps
        Stepper_Step(stepIndex);
        HAL_Delay(delay);  // Speed control
    }
}
