


#ifndef MOTORPOSITION_H
#define MOTORPOSITION_H

#include "servomotors.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"  // Adjust for your STM32 series
#include "config.h"

// variable definitionss

// Define GPIO pin numbers associated with motors
typedef struct {
   int motorPins[3];  // Stores 3 GPIO pins for 3 motors
} GPIOMotorPins;



// Probes & their home positions
typedef enum {
   RIGHT, // Right
   LEFT // Left
} ProbeSide;


// Structure to store home positions
typedef struct Position {
   int32_t x;
   int32_t y;
   int32_t z;
} Position;


typedef struct Probe {
    ProbeSide side;
    Position probePos;
    Servo* lin;
    Servo* rot;
    Stepper* nema;
    // Define GPIO pin numbers associated with stepper NEMA17
    

}Probe;


// Define home positions for each probe
const Position HOME_A = {0, 0, 0}; // need to edit
const Position HOME_B = {0, 0, 0}; // need to edit

// define offsets and distances based on workspace
#define PROBE_OFFSET_Z 10 // need to edit, tells distant from probe tip to workspace when in upright position
#define PROBE_LEN 20 // need to edit, in mm, length of the probe
#define THETA_MAX
#define THETA_MIN
#define Y_MAX // calculate based on workspace
#define Y_MIN 0
#define Z_MAX // calcuate based on workspace, how high up we want this motor to go
#define Z_MIN // calculate based on workspace, how low we want this motor to go

// Speed & scaling for motors
#define TIME_SCALE_Y 1 // how many seconds per centimeter
#define STEP_SCALE_Z 1
#define DEGREE_CONVERSION (180.0 / M_PI)
#define STEPPER_SPEED 10
#define STOP_SERVO_SPEED 0
#define HOME_THETA 0





// functions

void Y_align(Probe myProbe, Position desiredLoc);
void R_align(Probe myProbe, Position desiredLoc);
void Z_align(Probe myProbe, Position desiredLoc);
void home_Align(Probe myProbe, Position desiredLoc);
Position get_relative_pos(Probe myProbe, Position world_loc);
uint8_t get_motor_pin(Probe probe, MotorType motor);
void moveProbe(Probe myProbe, Position world_loc);


#ifdef __cplusplus
}
#endif

#endif /* SERVOMOTORS_H */

