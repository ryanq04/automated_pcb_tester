#ifndef SERVOMOTORS_H
#define SERVOMOTORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"  // Adjust for your STM32 series
// Datasheet link --> https://cdn-shop.adafruit.com/datasheets/PCA9685.pdf


// PCA9685 Register Definitions
#define PCA9685_MODE1         0x0         // as in the datasheet page no 10/52
#define PCA9685_PRE_SCALE     0xFE        // as in the datasheet page no 13/52
#define PCA9685_LED0_ON_L     0x6         // as in the datasheet page no 10/52

// PCA9685 Mode1 Bit Positions
#define PCA9685_MODE1_SLEEP_BIT      4    // as in the datasheet page no 14/52
#define PCA9685_MODE1_AI_BIT         5    // as in the datasheet page no 14/52
#define PCA9685_MODE1_RESTART_BIT    7    // as in the datasheet page no 14/52

// PCA9685 I2C Address
#define PCA9685_ADDRESS 0x80


// Function Prototypes
void PCA9685_SetBit(uint8_t Register, uint8_t Bit, uint8_t Value);
void PCA9685_SetPWMFrequency(uint16_t frequency);
void PCA9685_Init(uint16_t frequency);
void PCA9685_SetPWM(uint8_t Channel, uint16_t OnTime, uint16_t OffTime);
void PCA9685_SetServoAngle(uint8_t Channel, float Angle);
void FS90R_SetSpeed(uint8_t Channel, float Speed);

#ifdef __cplusplus
}
#endif

#endif /* SERVOMOTORS_H */
