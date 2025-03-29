#include <stdio.h>
#include <string.h>

#include "config.h"
#include "ov7670.h"

extern I2C_HandleTypeDef hi2c2;
extern DCMI_HandleTypeDef hdcmi;
extern DMA_HandleTypeDef hdma_dcmi;




extern uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];

const uint8_t OV7670_reg[OV7670_REG_NUM][2] = {
  
  // The array is formatted as: 
  // {<reg address>, <reg_value> }
  { 0x12, 0x80 },
  // Image format
  { 0x12, 0x8 },  // 0x14 = QVGA size, RGB mode; 0x8 = QCIF, YUV, 0xc = QCIF (RGB)
  { 0xc, 0x8 }, //
  { 0x11, 0b1000000 }, //

  // Hardware window
  { 0x11, 0x01 }, //PCLK settings, 15fps
  { 0x32, 0x80 }, //HREF
  { 0x17, 0x17 }, //HSTART
  { 0x18, 0x05 }, //HSTOP
  { 0x03, 0x0a }, //VREF
  { 0x19, 0x02 }, //VSTART
  { 0x1a, 0x7a }, //VSTOP

  // Scaling numbers
  { 0x70, 0x3a }, //X_SCALING
  { 0x71, 0x35 }, //Y_SCALING
  { 0x72, 0x11 }, //DCW_SCALING
  // { 0x73, 0xf0 }, //PCLK_DIV_SCALING
  { 0x73, 0xfa }, //PCLK_DIV_SCALING
  // { 0xa2, 0x02 }, //PCLK_DELAY_SCALING
  { 0xa2, 0x01 }, //PCLK_DELAY_SCALING

  // Matrix coefficients
  { 0x4f, 0x80 }, //
  { 0x50, 0x80 }, //
  { 0x51, 0x00 }, //
  { 0x52, 0x22 }, //
  { 0x53, 0x5e }, //
  { 0x54, 0x80 }, //
  { 0x58, 0x9e },

  // Gamma curve values
  { 0x7a, 0x20 }, //
  { 0x7b, 0x10 }, //
  { 0x7c, 0x1e }, //
  { 0x7d, 0x35 }, //
  { 0x7e, 0x5a }, //
  { 0x7f, 0x69 }, //
  { 0x80, 0x76 }, //
  { 0x81, 0x80 }, //
  { 0x82, 0x88 }, //
  { 0x83, 0x8f }, //
  { 0x84, 0x96 }, //
  { 0x85, 0xa3 }, //
  { 0x86, 0xaf }, //
  { 0x87, 0xc4 }, //
  { 0x88, 0xd7 }, //
  { 0x89, 0xe8 },

  // AGC and AEC parameters
  { 0xa5, 0x05 }, //
  { 0xab, 0x07 }, //
  { 0x24, 0x95 }, //
  { 0x25, 0x33 }, //
  { 0x26, 0xe3 }, //
  { 0x9f, 0x78 }, //
  { 0xa0, 0x68 }, //
  { 0xa1, 0x03 }, //
  { 0xa6, 0xd8 }, //
  { 0xa7, 0xd8 }, //
  { 0xa8, 0xf0 }, //
  { 0xa9, 0x90 }, //
  { 0xaa, 0x94 }, //
  { 0x10, 0x00 },

  // AWB parameters
  { 0x43, 0x0a }, //
  { 0x44, 0xf0 }, //
  { 0x45, 0x34 }, //
  { 0x46, 0x58 }, //
  { 0x47, 0x28 }, //
  { 0x48, 0x3a }, //
  { 0x59, 0x88 }, //
  { 0x5a, 0x88 }, //
  { 0x5b, 0x44 }, //
  { 0x5c, 0x67 }, //
  { 0x5d, 0x49 }, //
  { 0x5e, 0x0e }, //
  { 0x6c, 0x0a }, //
  { 0x6d, 0x55 }, //
  { 0x6e, 0x11 }, //
  { 0x6f, 0x9f }, //
  { 0x6a, 0x40 }, //
  { 0x01, 0x40 }, //
  { 0x02, 0x60 }, //
  { 0x13, 0xe7 },

  // Additional parameters
  { 0x34, 0x11 }, //
  { 0x3f, 0x00 }, //
  { 0x75, 0x05 }, //
  { 0x76, 0xe1 }, //
  { 0x4c, 0x00 }, //
  { 0x77, 0x01 }, //
  { 0xb8, 0x0a }, //
  { 0x41, 0x18 }, //
  { 0x3b, 0x12 }, //
  { 0xa4, 0x88 }, //
  { 0x96, 0x00 }, //
  { 0x97, 0x30 }, //
  { 0x98, 0x20 }, //
  { 0x99, 0x30 }, //
  { 0x9a, 0x84 }, //
  { 0x9b, 0x29 }, //
  { 0x9c, 0x03 }, //
  { 0x9d, 0x4c }, //
  { 0x9e, 0x3f }, //
  { 0x78, 0x04 }, //
  { 0x0e, 0x61 }, //
  { 0x0f, 0x4b }, //
  { 0x16, 0x02 }, //
  { 0x1e, 0x00 }, //
  { 0x21, 0x02 }, //
  { 0x22, 0x91 }, //
  { 0x29, 0x07 }, //
  { 0x33, 0x0b }, //
  { 0x35, 0x0b }, //
  { 0x37, 0x1d }, //
  { 0x38, 0x71 }, //
  { 0x39, 0x2a }, //
  { 0x3c, 0x78 }, //
  { 0x4d, 0x40 }, //
  { 0x4e, 0x20 }, //
  { 0x69, 0x00 }, //
  { 0x6b, 0x3a }, //
  { 0x74, 0x10 }, //
  { 0x8d, 0x4f }, //
  { 0x8e, 0x00 }, //
  { 0x8f, 0x00 }, //
  { 0x90, 0x00 }, //
  { 0x91, 0x00 }, //
  { 0x96, 0x00 }, //
  { 0x9a, 0x00 }, //
  { 0xb1, 0x0c }, //
  { 0xb2, 0x0e }, //
  { 0xb3, 0x82 }, //
  { 0x4b, 0x01 },
};


uint8_t ov7670_init(void){
  uint8_t val;
  char msg[100];

  print_msg("init_test\r\n");
  val = ov7670_read(0x0A);

  if (val != 0x76) {
    sprintf(msg, "Wrong product id (0x%x)\r\n", val);
    print_msg(msg);
    return 1;
  }else{
		sprintf(msg, "success\n");
    print_msg(msg);
	}

  // Your code here

  //

  return 0;
}

void read_all_ov7670_registers() {
    char msg[50];

    print_msg("Reading OV7670 Registers:\n");

    for (int i = 0; i < OV7670_REG_NUM; i++) {
        uint8_t reg_addr = OV7670_reg[i][0];  // Register address
        uint8_t reg_value = ov7670_read(reg_addr);  // Read register value

        sprintf(msg, "Reg 0x%02X = 0x%02X\n", reg_addr, reg_value);
        print_msg(msg);
    }

    print_msg("OV7670 Register Read Complete.\n");
}

void ov7_config(){
	char msg[20];
	uint8_t data[2];
	for(int i = 0; i < OV7670_REG_NUM; i++){ //for all rows of array, send an address and data frame corresponding to [i][0] and [i][1]
		data[0] = OV7670_reg[i][0]; 
		data[1] = OV7670_reg[i][1];
		HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c2, ADDR_OV7670, data, 2, 10000); //send address frame
		while(status != HAL_OK) {
			 sprintf(msg, "Error\n");
       print_msg(msg);
       HAL_Delay(1000);
       status = HAL_I2C_Master_Transmit(&hi2c2, ADDR_OV7670, data, 2, 10000); 
    }
		HAL_Delay(10);
	}
}

uint8_t ov7670_read(uint8_t reg){
    uint8_t data;
		char msg[100];
    // Transmit register
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c2, ADDR_OV7670, &reg, 1, 10000); 
    while(status != HAL_OK) {
			 sprintf(msg, "Error\n");
       print_msg(msg);
       HAL_Delay(1000);
       status = HAL_I2C_Master_Transmit(&hi2c2, ADDR_OV7670, &reg, 1, 10000); 
    }
    // Read data
    HAL_I2C_Master_Receive(&hi2c2, (reADDR_OV7670), &data, 1, HAL_MAX_DELAY);
    return data;
}




void ov7670_snapshot(uint16_t *buff){
  // Your code here
	char msg[100];
	HAL_StatusTypeDef status = HAL_DCMI_Start_DMA( &hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)snapshot_buff, BUFFER_SIZE/2); //SUS
	if(status != HAL_OK) {
			 sprintf(msg, "Error DMA failed\n");
       print_msg(msg);
    }
}//goes back to main for waiting for interrupt

void ov7670_capture(uint16_t *buff){
  // Your code here
	char msg[100];
	HAL_StatusTypeDef status = HAL_DCMI_Start_DMA( &hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)snapshot_buff, BUFFER_SIZE/2); //SUS
	if(status != HAL_OK) {
			 sprintf(msg, "Error DMA failed\n");
       print_msg(msg);
    }
}
