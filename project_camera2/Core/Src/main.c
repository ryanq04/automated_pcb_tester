
#include "main.h"
#include "ov2640.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

PCD_HandleTypeDef hpcd_USB_OTG_FS;



int main(void)
{
  
  HAL_Init();
  SystemClock_Config();

 
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DCMI_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();

  
  i2c_scan();
  ov2640_init();

  ov2640_config();
  read_all_ov2640_registers();


  while (1)
  {
	  
  }
}

