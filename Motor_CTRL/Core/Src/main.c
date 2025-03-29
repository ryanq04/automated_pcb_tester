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
#include "motorposition.h"

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

void servotest(){
   for(double i = 0; i < 180; i += 1){
        setServoAngle(15, i);
        HAL_Delay(10);

      }
      for(double i = 180; i > 0; i -= 1){
        setServoAngle(15, i);
        HAL_Delay(10);

      }
}


void actuateProbe(){

  setServoAngle(15, 15);
  FS90R_SetSpeed(0, -0.3);
  HAL_Delay(500);
  FS90R_SetSpeed(0, 0.3);
  HAL_Delay(500);
  FS90R_SetSpeed(0, 0);
  setServoAngle(15, 30);

  HAL_Delay(200);

}

void linearMove(){
    for(int i = 180; i >= 0; i -= 10){
      setServoAngle(0, i);
      HAL_Delay(500);
    }
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      
    HAL_Delay(2000);
    setServoAngle(0, 180);
    HAL_Delay(500);

}

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
  setServoAngle(0, 0);
  setServoAngle(15, 0);

  Servo lin;
  lin.Channel = 0;
  lin.currAngle = 0;

  Servo rot;
	rot.Channel = 15;
	rot.currAngle = 0;

	Stepper nema;
	stp_init(&nema);
 


	sv_moveDistance(&lin, 0);
	HAL_Delay(1000);
	setServoAngle(&rot, 0);
	int sign = -1;

  while (1)
  {
	  if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin))
	    {
			  start ^= 1;
			  HAL_Delay(200);
	  	}

	  // Stepper_Move(200, 10);
	  // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  // Stepper_Move(-200, 10);
	  // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);




	  if(start){
		  stp_moveDistance(&nema, sign * 1);
		  HAL_Delay(1000);
		  setServoAngle(&rot, 13.5);
		  HAL_Delay(1000);
		  sv_moveDistance(&lin, 0.55);


		  sign *= -1;
//		  stp_Move(&nema, 200, 10);
//		  setServoAngle(&rot, 34);
//		  HAL_Delay(1000);
//		  sv_moveDistance(&lin, 2.7);
		  start = 0;

	  }
    else{
    	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    	HAL_Delay(200);

    }
  }
}



/* Stepper motor test code

if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin))
	    {
			  start ^= 1;
			  HAL_Delay(200);
	  	}

if(start){
  Stepper_Move(200, 10);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}
      */

/*
Code to measure distance for servo
1) set servo angle to 0° --> mark the starting position of the slider (plug in the motor)
2) move servo in 10° increments --> measure how far the slider moves each time
3) create lookup table for each angle mapping to distance from start position 

4) create a linear equation: Position = m × Angle
  m = (pos_fin - pos_start)/ (deg_fin - def_start)
  angle = position / m 

#define  M1 // for channel 0 
#define  M2 // for channel 15
*/
