

#include "motorposition.h"
#include "steppermotors.h"
#include "servomotors.h"
#include "stdio.h"
#include "stdlib.h"
#include <math.h>

// Calculates the motor position based on given (x, y) coords

void home_Align(Probe myProbe){
    // 1. retract the motor
    setServoAngle(&(myProbe.lin), 0);
    myProbe.lin->currAngle = 0; 
    HAL_Delay(10); 

    // 2. rotate to home angle
    setServoAngle(&(myProbe.rot), 0);
    myProbe.rot->currAngle = 0; 
    HAL_Delay(10); 

    // 3. retract to home direction in stepper
    float move_cm = -(myProbe.nema->currAngle - HOME.x); // move right
    stp_moveDistance(&(myProbe.nema), move_cm);
    myProbe.nema->currAngle = myProbe.nema->homeAngle;
    HAL_Delay(10); 
}

void x_align(Probe myProbe, Position desiredLoc){
    float move_cm = -(HOME.x - desiredLoc.x);
    if(move_cm > X_MAX){
        move_cm = X_MAX;
    }
    stp_moveDistance(&((myProbe.nema)), move_cm);
    myProbe.nema->currAngle = move_cm; 
}

void theta_align(Probe myProbe, Position desiredLoc){
    float theta_rad = atan(desiredLoc.x / H);
    float theta_deg = theta_rad * DEGREE_CONVERSION;
    if(theta_deg > THETA_MAX){
        theta_deg = THETA_MAX;
    }
    setServoAngle(&(myProbe.rot), theta_deg);
    myProbe.rot->currAngle = theta_deg;
}

void R_align(Probe myProbe, Position desiredLoc){
    float hypotenuse = hypot(H, desiredLoc.y);
    float move_cm = hypotenuse - PROBE_LEN;
    sv_moveDistance(&(myProbe.lin), move_cm); // automatically updates cur_angle
    if(move_cm > DR_MAX){
        move_cm = DR_MAX; 
    }
}

// assume probe at home
void moveProbe_test(Probe myProbe, Position desiredLoc){

    // 1. send home
    home_Align(myProbe);

    // 2. align stepper, theta, R in order
    x_align(myProbe, desiredLoc);
    HAL_Delay(100); 
    theta_align(myProbe, desiredLoc);
    HAL_Delay(100); 
    R_align(myProbe, desiredLoc);
    HAL_Delay(100); 

    // 3. wait
    HAL_Delay(3000); 

    // 4. Align back home
    home_Align(myProbe);

}

void init_home(Probe myProbe){
    myProbe.nema->homeAngle = HOME.x;
    myProbe.nema->currAngle = myProbe.nema->homeAngle;

    myProbe.lin->currAngle = 0; 
    myProbe.lin->homeAngle = 0; 
    myProbe.rot->currAngle = 0; 
    myProbe.rot->homeAngle = 0; 

}

//input the base and height of the triangle that needs to be solved. Assumes that the Servo is sent home first 
//void moveProbe(float base, float height, Servo *lin, Servo *rot){
//    float theta_rad = atan((height)/base);
//    float theta_deg = theta_rad * DEGREE_CONVERSION;
//    float radius = base * base + height * height;
//    radius = sqrtf(radius);
//
//    float displacement = radius - RESTING_LENGTH;
//
//    sv_moveDistance(lin, displacement);
//    setServoAngle(rot, theta_deg);
//
//}
//
//void runInits(Servo* lin, Servo* rot, Stepper* nema){
//    sv_init(lin, 0, 0);
//    sv_init(rot, 0, 0);
//    stp_init(nema, MOTOR_PORT, IN1, IN2, IN3, IN4);
//}


// void initProbe(Probe* probe, ProbeSide side, Position homePosition, uint32_t R_Channel, uint32_t Th_Channel, GPIOStepperPins pins){
//     probe->side = side;
//     probe->probePos = homePosition;
//     probe->R_Channel = R_Channel;
//     probe->Th_Channel = Th_Channel;
    
//     for(int i = 0; i < 4; i++){
//         probe->gpiopins[i] = pins[i];
//     }
// }

// void Y_align(Probe myProbe, Position distance){
//     // STEP 1: controls the continuous servo motor (SERVO_MOTOR_Y) to travel the distance
//     // distance is relative posistion from home to the location we want 
//     float speed = (distance.y > 0) ? 1 : -1; // move forward or backward
    
//     int steps = distance.z * STEP_SCALE_Z;
//     Stepper_Move(steps, STEPPER_SPEED); // 10 = speed 

// }

// void Th_align(Probe myProbe, Position distance){
//     // STEP 2: controls the probe tip through the SERVO_MOTOR_R
//     float theta_rad = atan(distance.x / (PROBE_LEN)); // OR PROBE_OFFSET_Z + PROBE_LEN
//     float theta_deg = theta_rad * DEGREE_CONVERSION;

//     setServoAngle(myProbe->Th_Channel, theta_deg);
// }


// void Z_align(Probe myProbe, Position distance){
//     // STEP 3: lowers the probe down in the z-direction to touch the pad
//     uint8_t channel = get_motor_pin(myProbe, STEPPER_MOTOR);

    
    
// }

// void home_Align(Probe myProbe, Position currentLoc){
//     // currentLoc is the absolute position it is relative to original home position
//     Position movePos = {-currentLoc.x, -currentLoc.y, -currentLoc.z};

//     // 1. revert from the Z position   
//     Z_align(myProbe, movePos);
//     HAL_Delay(10); 

//     // 2. revert from the theta angle
//     uint8_t channel = get_motor_pin(myProbe, SERVO_MOTOR_R);
//     setServoAngle(channel, HOME_THETA);
//     HAL_Delay(10); 

//     // 3. revert from Y position
//     Y_align(myProbe, movePos);
//     HAL_Delay(10); 
// }

// Position get_relative_pos(Probe myProbe, Position world_loc){
//     // Takes in desired location of the probe in world coords and converts
//     // it to desired location relative to current probe

//     Position homePos = (myProbe == PROBE_A) ? HOME_A : HOME_B;
//     Position desiredLoc =  {world_loc.x - homePos.x, world_loc.y - homePos.y, world_loc.z - homePos.z};

//     return desiredLoc;
// }

// uint8_t get_motor_pin(Probe probe, MotorType motor) {
//     return motorPins[probe].motorPins[motor];
// }

// void moveProbe(Probe myProbe, Position world_loc){
//     // example of how to use the functions when giving global coords
//     Position desiredLoc = get_relative_pos(myProbe, world_loc);
//     Y_align(myProbe, desiredLoc);
//     HAL_Delay(100); 
//     R_align(myProbe, desiredLoc);
//     HAL_Delay(100); 
//     Z_align(myProbe, desiredLoc);
//     HAL_Delay(100); 
//     home_Align(myProbe, desiredLoc);
// }

