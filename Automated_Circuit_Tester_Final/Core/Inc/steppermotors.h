#ifndef STEPPERMOTORS_H
#define STEPPERMOTORS_H

#include "stm32f4xx_hal.h"
#include "config.h"

#define MOTOR_PORT GPIOF  // Choose an unused port that has all of the gpio pins below.

#define IN1 GPIO_PIN_12  // Connect to L298N IN1 from D8
#define IN2 GPIO_PIN_13  // Connect to L298N IN2 from D7
#define IN3 GPIO_PIN_14  // Connect to L298N IN3 from D4
#define IN4 GPIO_PIN_15  // Connect to L298N IN4 from D2
// Connection: on L298 1, 3, 2, 4 --> bottom to top on NIMA


#define NEMA_DISTANCE_TO_STEPS 49


typedef struct Stepper{
    float currAngle;
} Stepper;

void stp_Step(Stepper* motor, int step);
void stp_Move(Stepper* motor, int steps, int delay);
void stp_Stop(Stepper* motor);
void stp_moveDistance(Stepper* motor, float distance_cm);

#endif /* STEPPERMOTORS_H */
