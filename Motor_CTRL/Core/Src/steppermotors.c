
#include "steppermotors.h"
#include "stdlib.h"


void stp_Step(Stepper* motor, int step) {
    switch(step % 4) {
        case 0:
            HAL_GPIO_WritePin(motor->port, motor->motorPins[0], GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[1], GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[2], GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[3], GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(motor->port, motor->motorPins[0], GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[1], GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[2], GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[3], GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(motor->port, motor->motorPins[0], GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[1], GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[2], GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[3], GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(motor->port, motor->motorPins[0], GPIO_PIN_SET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[1], GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[2], GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor->port, motor->motorPins[3], GPIO_PIN_SET);
            break;
    }
}



void stp_init(Stepper* nema, int port, int in1, int in2, int in3, int in4){
    nema->port = port;
    GPIOStepperPins pins = {in1, in2, in3, in4}
    nema->pins = pins;
    nema->homeAngle = 0;
    nema->currAngle = 0;
}


void stp_Stop(Stepper* motor) {
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(motor->port, motor->motorPins[i], GPIO_PIN_RESET);
    }
}

void stp_Move(Stepper* motor, int steps, int delay) {
    int direction = (steps > 0) ? 1 : -1; // Determine direction
    int stepCount = abs(steps);  // Get absolute step count

    for (int i = 0; i < stepCount; i++) {
        int stepIndex = (direction > 0) ? (i % 4) : (3 - (i % 4));
        stp_Step(motor, stepIndex);
        HAL_Delay(delay); // Speed control
    }
    stp_Stop(motor);
}

void stp_moveDistance(Stepper* motor, float distance_cm){
    
    int convertedSteps = distance_cm * NEMA_DISTANCE_TO_STEPS;
    stp_Move(motor, )
}