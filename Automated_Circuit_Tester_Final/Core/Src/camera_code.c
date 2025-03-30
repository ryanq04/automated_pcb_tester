
#include "main.h"
#include "config.h"

#include "ov7670.h"


#include <stdio.h>
#include <string.h>



extern uint16_t snapshot_buff[IMG_ROWS * IMG_COLS] ;
extern uint8_t send_ptr[FRAMESIZE * 2];
extern uint8_t dma_flag;


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
	uint8_t pream[] = "\r\nPREAMBLE!\r\n"; 

	uint8_t* byte_ptr = (uint8_t*)snapshot_buff;

	for (int i = FRAMESIZE * 2 - 1, j = 0; i >= 0 && j < FRAMESIZE*2-1; i--, j++) {
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

	//SEND PREAMBLE
	for(int i = 0; i < 13; i++){ 
		HAL_UART_Transmit(&huart3, &pream[i], 1, HAL_MAX_DELAY);
	}
	
	// Send image data through serial port.
	
	HAL_UART_Transmit(&huart3, send_ptr, FRAMESIZE*2, HAL_MAX_DELAY);
	//HAL_UART_Transmit(&huart3, &send_ptr[FRAMESIZE], FRAMESIZE, HAL_MAX_DELAY);
	
}


void p3(){
	ov7670_snapshot(snapshot_buff);
		while(!dma_flag)
		{
			HAL_Delay(10);	
		}
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
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


//{
//
//	__HAL_DMA_ENABLE_IT(&hdma_dcmi, DMA_IT_TC);
//	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
//
//  char msg[100];
//  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
//  ov7670_init();
//  ov7_config();
//
//
//  while (1)
//  {
//		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
//		HAL_Delay(100);
//
//    if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
//			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
//			HAL_Delay(100);
//
//			//p3();
//			p4();
//    }
//  }
//}
//

