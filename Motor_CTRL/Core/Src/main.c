/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "steppermotors.h"
#include "servomotors.h"
#include "config.h"

 I2C_HandleTypeDef hi2c2;
 UART_HandleTypeDef huart3;
 DMA_HandleTypeDef hdma_usart3_tx;
 PCD_HandleTypeDef hpcd_USB_OTG_FS;
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/



/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */


int main(void)
{

  HAL_Init();

  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();

  /* USER CODE BEGIN 2 */
  PCA9685_Init(50); // 50Hz for servo
  /* USER CODE END 2 */

  int start = 0;

  while (1)
  {
	  if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin))
	    {
			  start ^= 1;
			  HAL_Delay(200);
	  	}

	  if(start){
		  Stepper_Move(200, 10);
		  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  }
  }
}


