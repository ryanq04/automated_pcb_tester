#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern DCMI_HandleTypeDef hdcmi;
extern DMA_HandleTypeDef hdma_dcmi;

extern I2C_HandleTypeDef hi2c2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_tx;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

void SystemClock_Config(void);
 void MX_GPIO_Init(void);
 void MX_DMA_Init(void);
 void MX_DCMI_Init(void);
 void MX_USART3_UART_Init(void);
 void MX_USB_OTG_FS_PCD_Init(void);
 void MX_I2C2_Init(void);
 void MX_TIM1_Init(void);
 void MX_TIM6_Init(void);
void print_msg(char * msg);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

#define FRAMEX 160
#define FRAMEY 120

#define BUFFER_SIZE (FRAMEX * FRAMEY)
#define FRAMESIZE (FRAMEX * FRAMEY)

#endif /* __CONFIG_H */
