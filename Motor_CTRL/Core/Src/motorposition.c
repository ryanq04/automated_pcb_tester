#include "motorposition.h"
#include "servomotors.h"
#include "stdio.h"
#include "stdlib.h"
#include <math.h>

void Y_align(Probe myProbe, Position desiredLoc){
    // STEP 1: controls the continuous servo motor (SERVO_MOTOR_Y) to reach the desiredLoc
    // desiredLoc is relative posistion from home to the location we want 
    
    uint8_t channel = get_motor_pin(myProbe, SERVO_MOTOR_Y);
    uint16_t delay = desiredLoc.y * TIME_SCALE_Y; // for how long we should drive this motor
    float speed = (desiredLoc.y > 0) ? 1 : -1; // move forward or backward
    
    FS90R_SetSpeed(channel, speed);
    HAL_Delay(delay);
    FS90R_SetSpeed(channel, STOP_SERVO_SPEED); // stop moving, STOP_SERVO_ANGLE = 0, sends Angle 90°
}

void R_align(Probe myProbe, Position desiredLoc){
    // STEP 2: controls the probe tip through the SERVO_MOTOR_R
    float theta_rad = atan(desiredLoc.x / (PROBE_LEN)); // OR PROBE_OFFSET_Z + PROBE_LEN
    float theta_deg = theta_rad * DEGREE_CONVERSION;

    uint8_t channel = get_motor_pin(myProbe, SERVO_MOTOR_R);
    setServoAngle(channel, theta_deg);
}

void Z_align(Probe myProbe, Position desiredLoc){
    // STEP 3: lowers the probe down in the z-direction to touch the pad
    uint8_t channel = get_motor_pin(myProbe, STEPPER_MOTOR);

    int steps = desiredLoc.z * STEP_SCALE_Z;
    Stepper_Move(steps, STEPPER_SPEED); // 10 = speed 
}

void home_Align(Probe myProbe, Position desiredLoc){
    // desiredLoc is the previous position it had relative to original home position
    Position movePos = {-desiredLoc.x, -desiredLoc.y, -desiredLoc.z};

    // 1. revert from the Z position   
    Z_align(myProbe, movePos);
    HAL_Delay(10); 

    // 2. revert from the theta angle
    uint8_t channel = get_motor_pin(myProbe, SERVO_MOTOR_R);
    setServoAngle(channel, HOME_THETA);
    HAL_Delay(10); 

    // 3. revert from Y position
    Y_align(myProbe, movePos);
    HAL_Delay(10); 
}

Position get_relative_pos(Probe myProbe, Position world_loc){
    // Takes in desired location of the probe in world coords and converts
    // it to desired location relative to current probe

    Position homePos = (myProbe == PROBE_A) ? HOME_A : HOME_B;
    Position desiredLoc =  {world_loc.x - homePos.x, world_loc.y - homePos.y, world_loc.z - homePos.z};

    return desiredLoc;
}

uint8_t get_motor_pin(Probe probe, MotorType motor) {
    return motorPins[probe].motorPins[motor];
}

void moveProbe(Probe myProbe, Position world_loc){
    // example of how to use the functions when giving global coords
    Position desiredLoc = get_relative_pos(myProbe, world_loc);
    Y_align(myProbe, desiredLoc);
    HAL_Delay(100); 
    R_align(myProbe, desiredLoc);
    HAL_Delay(100); 
    Z_align(myProbe, desiredLoc);
    HAL_Delay(100); 
    home_Align(myProbe, desiredLoc);
}