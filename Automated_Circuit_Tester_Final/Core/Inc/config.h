#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "main.h"








extern ADC_HandleTypeDef hadc1; //adc 
extern DMA_HandleTypeDef hdma_adc1; //adc
extern TIM_HandleTypeDef htim2; //adc


extern DCMI_HandleTypeDef hdcmi; //cam
extern DMA_HandleTypeDef hdma_dcmi; //cam 

extern I2C_HandleTypeDef hi2c1; //motors
extern I2C_HandleTypeDef hi2c2; //cam

extern TIM_HandleTypeDef htim1; //cam
extern TIM_HandleTypeDef htim6; //cam

extern UART_HandleTypeDef huart3; //adc, cam, fsm


// Function declarations for system/peripheral initialization


void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_DCMI_Init(void);
void MX_USART3_UART_Init(void);
void MX_I2C2_Init(void);
void MX_TIM1_Init(void);
void MX_TIM6_Init(void);
void MX_ADC1_Init(void);
void MX_I2C1_Init(void);
void MX_TIM2_Init(void);
void MX_USB_OTG_FS_USB_Init(void);



// Optional Error Handler declaration
void Error_Handler(void);
void print_msg(char * msg);


#endif /* __CONFIG_H__ */
