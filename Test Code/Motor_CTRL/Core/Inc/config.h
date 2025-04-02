#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"  // Include necessary STM32 HAL headers
#include "main.h"

/* Peripheral Handles */
 extern I2C_HandleTypeDef hi2c2;
 extern UART_HandleTypeDef huart3;
 extern DMA_HandleTypeDef hdma_usart3_tx;
 extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* Function Prototypes */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_USART3_UART_Init(void);
void MX_USB_OTG_FS_PCD_Init(void);
void MX_I2C2_Init(void);

#endif /* CONFIG_H */
