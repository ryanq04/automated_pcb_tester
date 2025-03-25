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

#define FRAMESIZE IMG_ROWS * IMG_COLS
#define BUFFER_SIZE IMG_ROWS * IMG_COLS


uint16_t snapshot_buff[IMG_ROWS * IMG_COLS] = {0};
uint8_t send_ptr_rgb[FRAMESIZE*2] = {0}; // Each pixel needs 16 bits (2 bytes)


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


void send_img_rgb565(){
	uint8_t pream[] = "\r\nPREAMBLE!\r\n"; uint8_t end[] = "\r\nEND!\r\n";
	char msg[100];
	
	uint16_t* byte_ptr = (uint16_t*)snapshot_buff;

	for (int i = FRAMESIZE - 1, j = 0; (i >= 0) && (j < FRAMESIZE * 2); i--, j += 2) {
		send_ptr_rgb[j+1] = (byte_ptr[i] >> 8) & 0xFF; //High byte
		send_ptr_rgb[j] = byte_ptr[i] & 0xFF; // Low byte
	}
	

	//SEND PREAMBLE
	for(int i = 0; i < 13; i++){ 
		HAL_UART_Transmit(&huart3, &pream[i], 1, HAL_MAX_DELAY);
	}

	for(int i = 0; i < FRAMESIZE*2; i++){
		HAL_UART_Transmit(&huart3, &send_ptr_rgb[i], 1, HAL_MAX_DELAY);
	}


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
		//send_img(); 
		//send_img_ycbcr();
		//send_cb_image();
		//send_img_rgb();
		send_img_rgb565();
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
		//send_img();
		send_img_rgb565();
	}
}

void create_red_img(){
	for(int i = 0; i < IMG_COLS*IMG_ROWS / 3; i = i+3){
		snapshot_buff[i] = 0xf800;
		snapshot_buff[i+1] = 0x07e0;
		snapshot_buff[i+2] = 0x001f;

	}

}

void create_red_green_blue_img(){
    int stripe_width = 5;  // Adjust this for thicker or thinner stripes
    for(int i = 0; i < IMG_COLS * IMG_ROWS; i++){
        if ((i / stripe_width) % 3 == 0) {
            snapshot_buff[i] = 0xF800;  // Red
        } else if ((i / stripe_width) % 3 == 1) {
            snapshot_buff[i] = 0x07E0;  // Green
        } else {
            snapshot_buff[i] = 0x001F;  // Blue
        }
    }
}

void create_vertical_stripes_img() {
    int stripe_width = 5;  // Adjust this to make the stripes thicker or thinner
    for(int i = 0; i < IMG_ROWS; i++) {
        for(int j = 0; j < IMG_COLS; j++) {
            uint16_t color = 0x0000;  // Default (black) color

            // Determine which stripe (Red, Green, Blue) the current pixel belongs to based on the column (j)
            if ((j / stripe_width) % 3 == 0) {
                // Red stripe (5 bits for Red)
                color = 0xF800;  // 0xF800 is RGB565 for Red (11111 000000 00000)
            } else if ((j / stripe_width) % 3 == 1) {
                // Green stripe (6 bits for Green)
                color = 0x07E0;  // 0x07E0 is RGB565 for Green (00000 111111 00000)
            } else {
                // Blue stripe (5 bits for Blue)
                color = 0x001F;  // 0x001F is RGB565 for Blue (00000 000000 11111)
            }

            // Assign the color to the correct pixel position in the buffer
            snapshot_buff[i * IMG_COLS + j] = color;
        }
    }
}

void create_checkerboard_img() {
    int box_size = 10;  // Size of each checkerbox (adjust this to control the size of the squares)

    for(int i = 0; i < IMG_ROWS; i++) {
        for(int j = 0; j < IMG_COLS; j++) {
            uint16_t color = 0x0000;  // Default (black) color

            // Determine if the current position (i, j) is in a purple or yellow box
            if (((i / box_size) + (j / box_size)) % 2 == 0) {
                // Purple box (combination of Red and Blue)
                color = 0xb01a;  // 0x7F00 is RGB565 for Purple
            } else {
                // Yellow box (combination of Red and Green)
                color = 0x9dfb;  // 0xFFE0 is RGB565 for Yellow
            }

            // Assign the color to the correct pixel position in the buffer
            snapshot_buff[i * IMG_COLS + j] = color;
        }
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
			//create_checkerboard_img();
			//send_img_rgb565();
    }
  }
}


