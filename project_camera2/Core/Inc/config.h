#ifndef __CONFIG_H
#define __CONFIG_H

#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DCMI_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM6_Init(void);
void print_msg(char * msg);

#endif /* __CONFIG_H */