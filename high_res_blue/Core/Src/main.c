/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "main.h"
#include "config.h"

#include "ov7670.h"


#include <stdio.h>
#include <string.h>

#define FRAMESIZE 25056
#define BUFFER_SIZE 25056


uint16_t snapshot_buff[IMG_ROWS * IMG_COLS] = {0};
uint8_t send_ptr[FRAMESIZE] = {0};
//uint32_t DMA_Data[BUFFER_SIZE/2] = {0};

uint8_t dma_flag = 0;


void print_buf(void);
void print_bb(uint8_t *byte_ptr){
	char msg[100];
	sprintf(msg, "\n***The following is data for my byte buffer\n");
	print_msg(msg);
	int ct = 0;
	for(int i = 0; i < BUFFER_SIZE * 2; i++){
		if(i % 174 == 0){
			sprintf(msg, "\n%d***\n", ct++);
			print_msg(msg);
		}
		sprintf(msg, "%02X ", byte_ptr[i]);
		print_msg(msg);
	}


}
void print_sb(uint8_t *send_ptr){
	char msg[100];
	sprintf(msg, "***The following is data for my sender buffer\n");
	print_msg(msg);
	int ct = 0;
	for(int i = 0; i < BUFFER_SIZE; i++){
		if(i % 174 == 0){
			sprintf(msg, "\n%d***\n", ct++);
			print_msg(msg);
		}
		sprintf(msg, "%02X ", send_ptr[i]);
		print_msg(msg);
	}
}
void print_ss() {
	
	
  // Send image data through serial port.
	char msg[100];
	int ct = 0;
	sprintf(msg, "***The following is data for my snapshot buffer: \n");
	print_msg(msg);
	for(int i = 0; i < BUFFER_SIZE; i++){
		if(i % 174 == 0){
			sprintf(msg, "\n%d***\n", ct++);
			print_msg(msg);
		}
		sprintf(msg, "%04X ", snapshot_buff[i]);
		print_msg(msg);
	}
}


void send_frame() {
	uint8_t pream[] = "\r\nPREAMBLE!\r\n";
  uint8_t zero = 0xFF;
	for(int i = 0; i < 13; i++){
		HAL_UART_Transmit(&huart3, &pream[i], 1, HAL_MAX_DELAY);
	}
  for (int i = 0; i < FRAMESIZE; i++) {
		zero = 0x00 + i;
    HAL_UART_Transmit(&huart3, &zero, 1, HAL_MAX_DELAY);
  }
	uint8_t end[] = "\r\nEND!\r\n";
	for(int i = 0; i < 8; i++){
		HAL_UART_Transmit(&huart3, &end[i], 1, HAL_MAX_DELAY);
	}
}


void send_img(){
	uint8_t pream[] = "\r\nPREAMBLE!\r\n"; uint8_t end[] = "\r\nEND!\r\n";

	uint8_t* byte_ptr = (uint8_t*)snapshot_buff;

	for (int i = FRAMESIZE * 2 - 1, j = 0; i >= 0 && j < FRAMESIZE; i-=2, j++) {
    	send_ptr[j] = byte_ptr[i];
  	}

	//#define DEBUG  

	#ifdef DEBUG
		#define LOG() do { \
			print_bb(byte_ptr); \
			print_sb(send_ptr); \
		} while (0)
	#else
		#define LOG() do { } while (0)
	#endif

	LOG();

	//SEND PREAMBLE
	for(int i = 0; i < 13; i++){ 
		HAL_UART_Transmit(&huart3, &pream[i], 1, HAL_MAX_DELAY);
	}
	
	// Send image data through serial port.
	
	HAL_UART_Transmit(&huart3, send_ptr, FRAMESIZE/2, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3, &send_ptr[FRAMESIZE/2], FRAMESIZE/2, HAL_MAX_DELAY);
	
	//SEND SUFFIX
	for(int i = 0; i < 7; i++){
		HAL_UART_Transmit(&huart3, &end[i], 1, HAL_MAX_DELAY);
	}
}


void p3(){
	ov7670_snapshot(snapshot_buff);
		while(!dma_flag)
		{
			HAL_Delay(10);	
		}
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		dma_flag = 0;
		//print_ss(); 
		send_img(); 
}

void p4(){
	while(1){
		HAL_DCMI_Suspend(&hdcmi);
		ov7670_capture(snapshot_buff);
		while(!dma_flag)
		{
			HAL_Delay(10);	
		}
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		dma_flag = 0;
		HAL_DCMI_Resume(&hdcmi);
		send_img(); 
	}
}



int main(void)
{
  /* Reset of all peripherals */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DCMI_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
	
	__HAL_DMA_ENABLE_IT(&hdma_dcmi, DMA_IT_TC);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

  char msg[100];
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  ov7670_init();
	ov7_config();


  while (1)
  {
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		HAL_Delay(100);
    
    if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			HAL_Delay(100);
			
			//p3();
			p4();
    }
  }
}


