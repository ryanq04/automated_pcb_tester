#ifndef MOTORPOSITION_H
#define MOTORPOSITION_H
//
#include "servomotors.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"  // Adjust for your STM32 series
#include "config.h"
#include "steppermotors.h"
#include "servomotors.h"

// variable definitionss

// Define GPIO pin numbers associated with motors
typedef struct {
  int motorPins[3];  // Stores 3 GPIO pins for 3 motors
} GPIOMotorPins;


// Structure to store home positions
typedef struct Position {
  float x;
  float y;
  float z;
} Position;


typedef struct Probe {
   Position probePos;
   Servo* lin;
   Servo* rot;
   Stepper* nema;
   // Define GPIO pin numbers associated with stepper NEMA17
}Probe;


// Define home positions for each probe
extern const Position HOME; // probe home, 3 mm off the ground

// remember that LEFT --> -ve, RIGHT --> +ve for stepper

// define offsets and distances based on workspace
#define H 12.5 // Z-axis height from ground
#define PROBE_LEN 12.3 // need to edit, in mm, length of the probe
#define THETA_MAX 34.0
#define X_MAX 13.0 // max distance stepper can move
#define DR_MAX 2.7

// Speed & scaling for motors
#define DEGREE_CONVERSION (180.0 / M_PI)

// functions
void home_Align(Probe* myProbe);
void x_align(Probe* myProbe, Position desiredLoc);
void theta_align(Probe* myProbe, Position desiredLoc);
void R_align(Probe* myProbe, Position desiredLoc);
void moveProbe_test(Probe* myProbe, Position desiredLoc);
void init_home(Probe* myProbe);

#ifdef __cplusplus
}
#endif

#endif /* MOTORPOSITION_H */

