#include "motorposition.h"
#include "steppermotors.h"
#include "servomotors.h"
#include "stdio.h"
#include "stdlib.h"
#include <math.h>

extern const Position HOME;

// Calculates the motor position based on given (x, y) coords
void home_Align(Probe* myProbe){
    // 1. retract the motor
    setServoAngle(myProbe->lin, myProbe->lin->homeAngle);
    myProbe->lin->currAngle = myProbe->rot->homeAngle; 
    HAL_Delay(500); 

    // 2. rotate to home angle
    setServoAngle(myProbe->rot, myProbe->rot->homeAngle);
    myProbe->rot->currAngle = myProbe->rot->homeAngle; 
    HAL_Delay(500); 

    // 3. retract to home direction in stepper
    float move_cm = -(myProbe->nema->currAngle - HOME.x); // move right
    stp_moveDistance((myProbe->nema), move_cm);
    myProbe->nema->currAngle = myProbe->nema->homeAngle;
    HAL_Delay(500); 
}

void x_align(Probe* myProbe, Position desiredLoc){
    flashLED(LD1_GPIO_Port, LD1_Pin, 1000, 5);
    float move_cm = -(HOME.x - desiredLoc.x);
    if(move_cm > X_MAX){
        move_cm = X_MAX;
    }
    stp_moveDistance(((myProbe->nema)), move_cm);
    myProbe->nema->currAngle = move_cm; 
}

void theta_align(Probe* myProbe, Position desiredLoc){
    float theta_rad = atan(fabsf(HOME.y -desiredLoc.y) / H);
    float theta_deg = theta_rad * DEGREE_CONVERSION;
    if(theta_deg > THETA_MAX){
        theta_deg = THETA_MAX;
    }
    setServoAngle(myProbe->rot, theta_deg);
    myProbe->rot->currAngle = theta_deg;
}

void R_align(Probe* myProbe, Position desiredLoc){
    float hypotenuse = hypot(H, abs(HOME.y - desiredLoc.y));
    float move_cm = hypotenuse - PROBE_LEN;
    if(move_cm > DR_MAX){
        move_cm = DR_MAX; 
    }
    sv_moveDistance(myProbe->lin, move_cm); // automatically updates cur_angle
}

// assume probe at home
void moveProbe_test(Probe* myProbe, Position desiredLoc){

    // 1. send home
    home_Align(myProbe);

    // 2. align stepper, theta, R in order
    //x_align(myProbe, desiredLoc);
    HAL_Delay(500); 
    theta_align(myProbe, desiredLoc);
    HAL_Delay(500); 
    R_align(myProbe, desiredLoc);
    HAL_Delay(500); 

    // 3. wait
    HAL_Delay(3000); 

    // 4. Align back home
    //home_Align(myProbe);

}

void init_home(Probe* myProbe){
    myProbe->nema->homeAngle = HOME.x;
    myProbe->nema->currAngle = myProbe->nema->homeAngle;

    myProbe->lin->currAngle = 0; 
    myProbe->lin->homeAngle = 0; 
    myProbe->rot->currAngle = 0; 
    myProbe->rot->homeAngle = 0; 

}

