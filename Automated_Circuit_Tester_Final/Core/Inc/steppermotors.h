#ifndef STEPPERMOTORS_H
#define STEPPERMOTORS_H

#include "stm32f4xx_hal.h"
#include "config.h"

#define MOTOR_PORT GPIOA  // Choose an unused port that has all of the gpio pins below.

#define IN1 GPIO_PIN_3  // Connect to L298N IN1 from A0
#define IN2 GPIO_PIN_5  // Connect to L298N IN2 from D13
#define IN3 GPIO_PIN_6  // Connect to L298N IN3 from D12
#define IN4 GPIO_PIN_7  // Connect to L298N IN4 from D11

#define NEMA_DISTANCE_TO_STEPS 49


typedef struct Stepper{
    float homeAngle;
    float currAngle;
} Stepper;

void stp_init(Stepper* nema);
void stp_Step(Stepper* motor, int step);
void stp_Move(Stepper* motor, int steps, int delay);
void stp_Stop(Stepper* motor);
void stp_moveDistance(Stepper* motor, float distance_cm);

#endif /* STEPPERMOTORS_H */
