
#include "servomotors.h"

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void PCA9685_SetBit(uint8_t Register, uint8_t Bit, uint8_t Value)
{
  uint8_t readValue;
  // Read all 8 bits and set only one bit to 0/1 and write all 8 bits back
  HAL_I2C_Mem_Read(&hi2c1, PCA9685_ADDRESS, Register, 1, &readValue, 1, 10);
  if (Value == 0) readValue &= ~(1 << Bit);
  else readValue |= (1 << Bit);
  HAL_I2C_Mem_Write(&hi2c1, PCA9685_ADDRESS, Register, 1, &readValue, 1, 10);
  HAL_Delay(1);
}

void PCA9685_SetPWMFrequency(uint16_t frequency)
{
  uint8_t prescale;
  if(frequency >= 1526) prescale = 0x03; // why?
  else if(frequency <= 24) prescale = 0xFF;
  //  internal 25 MHz oscillator as in the datasheet page no 1/52
  else prescale = 25000000 / (4096 * frequency);
  // prescale changes 3 to 255 for 1526Hz to 24Hz as in the datasheet page no 1/52
  PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_SLEEP_BIT, 1);
  HAL_I2C_Mem_Write(&hi2c1, PCA9685_ADDRESS, PCA9685_PRE_SCALE, 1, &prescale, 1, 10);
  PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_SLEEP_BIT, 0);
  PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_RESTART_BIT, 1);
}

void PCA9685_Init(uint16_t frequency)
{
  PCA9685_SetPWMFrequency(frequency); // 50 Hz for servo
  PCA9685_SetBit(PCA9685_MODE1, PCA9685_MODE1_AI_BIT, 1);
}

void PCA9685_SetPWM(uint8_t Channel, uint16_t OnTime, uint16_t OffTime)
{
  uint8_t registerAddress;
  uint8_t pwm[4];
  registerAddress = PCA9685_LED0_ON_L + (4 * Channel);
  // See example 1 in the datasheet page no 18/52
  pwm[0] = OnTime & 0xFF;
  pwm[1] = OnTime>>8;
  pwm[2] = OffTime & 0xFF;
  pwm[3] = OffTime>>8;
  HAL_I2C_Mem_Write(&hi2c1, PCA9685_ADDRESS, registerAddress, 1, pwm, 4, 10);
}

void setServoAngle_r(uint8_t Channel, float Angle)
{
  float Value;

  // (pulse width / 20 ms) * 4096 = PCA pwm value
  // 
  Value = (205.0 + (Angle / 180.0) * (410.0 - 205.0));
  PCA9685_SetPWM(Channel, 0, (uint16_t)Value);
}

void setServoAngle(Servo* sv, float Angle)
{
  // this one is for the position
  float Value;
    if (Angle < 0) Angle = 0;
    if (Angle > 180) Angle = 180;

    float Rev_Angle = 180 - Angle; //in order to make positive values forward and negative values backwards ;)

  Value = (Rev_Angle * (511.9 - 102.4) / 180.0) + 102.4;
  PCA9685_SetPWM(sv->Channel, 0, (uint16_t)Value);
}

/*
1ms CW FULLSPEED
1.5 ms STOP
2 ms CCW FULLSPEED
*/
void FS90R_SetSpeed(uint8_t Channel, float speed)
{
    if (speed < -1.0) speed = -1.0;
    if (speed > 1.0) speed = 1.0;

    //Speed passed in from (-1,1), representing max and mininum range of speed 
    float Angle = speed * 90 + 90;
    setServoAngle_r(Channel, Angle);
}

/*
void FS90R_move_linear_distance(uint8_t Channel, float distance){

    FS90R_SetSpeed(channel, speed);
    HAL_Delay(DIST_OVER_SPEED_RATIO); // d = vt so we need to calulate the time needed to travel a distance 
    FS90R_SetSpeed(channel, STOP_SERVO_SPEED); // stop moving, STOP_SERVO_ANGLE = 0, sends Angle 90°


}
*/

void sv_init(Servo* sv, uint8_t Channel, float homeAngle){
  sv->Channel = Channel;
  sv->homeAngle = homeAngle;
  setServoAngle(sv, homeAngle);
}

//pass in the current Angle of the servo, and modify that value by its angle equivalent of the distance 
void sv_moveDistance(Servo* sv, float distance_cm) {

  //theoretically we have around 2.8cm per 180 deg
  //experimentally we have around 2.52cm per 180 deg. 

    float delta_angle = distance_cm / 0.01544; // (0.014)

    if(delta_angle + sv->currAngle > 0 && delta_angle + sv->currAngle <= 180){
      sv->currAngle += delta_angle;
    }else{
      //print_msg("Angle change request exceeded bounds");
      HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

    }

    setServoAngle(sv, sv->currAngle);
  
}

void sv_sendHome(Servo *lin, Servo *rot){
    setServoAngle(lin, lin->homeAngle);
    setServoAngle(rot, rot->homeAngle);
}


// Code to measure distance for servo
// 1) set servo angle to 0° --> mark the starting position of the slider (plug in the motor)
// 2) move servo in 10° increments --> measure how far the slider moves each time
// 3) create lookup table for each angle mapping to distance from start position 

// 4) create a linear equation: Position = m × Angle
//   m = (pos_fin - pos_start)/ (deg_fin - def_start)
//   angle = position / m 

// #define  M1 // for channel 0 
// #define  M2 // for channel 15





