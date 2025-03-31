
#include "steppermotors.h"
#include "stdlib.h"


void stp_Step(Stepper* motor, int step) {
   switch(step % 4) {
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



void stp_init(Stepper* nema){
   nema->currAngle = 0;
}


void stp_Stop(Stepper* motor) {
	HAL_GPIO_WritePin(MOTOR_PORT, IN1, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(MOTOR_PORT, IN2, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(MOTOR_PORT, IN3, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(MOTOR_PORT, IN4, GPIO_PIN_RESET);

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
	int sign = 1;
    if(distance_cm < 0.0){
        sign = -1;
        flashLED(LD3_GPIO_Port, LD3_Pin, 500, 10);
    }
    
   int convertedSteps = sign * abs(distance_cm * NEMA_DISTANCE_TO_STEPS);
   stp_Move(motor, convertedSteps, 5);
}
