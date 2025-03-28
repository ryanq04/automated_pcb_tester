#ifndef STEPPERMOTORS_H
#define STEPPERMOTORS_H

#include "stm32f4xx_hal.h"
#include "config.h"

#define MOTOR_PORT GPIOA  // Choose an unused port that has all of the gpio pins below.

#define IN1 GPIO_PIN_3  // Connect to L298N IN1 from A0
#define IN2 GPIO_PIN_5  // Connect to L298N IN2 from D13
#define IN3 GPIO_PIN_6  // Connect to L298N IN3 from D12
#define IN4 GPIO_PIN_7  // Connect to L298N IN4 from D11

#define NEMA_DISTANCE_TO_STEPS 67

typedef struct GPIOStepperPins{
       int motorPins[4];  // Stores 4 GPIO pins for the NEMA17
} GPIOStepperPins;


typedef struct Stepper{
    int port;
    GPIOStepperPins motorPins;
    float homeAngle;
    float currAngle;
} Stepper;

void stp_init(Stepper* nema, int port, int in1, int in2, int in3, int in4);
void stp_Step(Stepper* motor, int step);
void stp_Move(Stepper* motor, int steps, int delay);
void stp_Stop(Stepper* motor);

#endif /* STEPPERMOTORS_H */
