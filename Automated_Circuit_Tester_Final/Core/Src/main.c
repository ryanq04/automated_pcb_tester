
#include "config.h"
#include "main.h"
#include <stddef.h>
#include <stdio.h>
#include <math.h>


#include "adc_fft.h"
#include "ov7670.h"
#include "motorposition.h"

#include "fsm.h"



ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;
UART_HandleTypeDef huart3;

//ADC_FFT globals
uint16_t adc_buffer[ADC_BUF_LEN];
uint8_t ADC_full = 0;
uint8_t sendFFT_ready = 0;
float input_FFT[FFT_BUFFER_SIZE];
float output_FFT[FFT_BUFFER_SIZE];
arm_rfft_fast_instance_f32 fftHandler;

//camera globals
uint16_t snapshot_buff[IMG_ROWS * IMG_COLS] = {0};
uint8_t send_ptr[FRAMESIZE * 2] = {0};
uint8_t dma_flag = 0;

//fsm globals
StateFunc current_state = NULL;


//init motor globals
Probe myProbe; 
Servo lin;
Servo rot;
Stepper nema;
const Position HOME = {9, 8, 12.2};
	

int main(void)
{
	HAL_Init();
  	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_DCMI_Init();
	MX_USART3_UART_Init();
	MX_I2C2_Init();
	MX_TIM1_Init();
	MX_TIM6_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_TIM2_Init();
	MX_USB_OTG_FS_USB_Init();
	
	//init FFT
	arm_rfft_fast_init_f32(&fftHandler, FFT_BUFFER_SIZE);

	//init probe
	myProbe.lin = &lin;
	myProbe.rot = &rot;
	myProbe.nema = &nema;
	
    myProbe.probePos = HOME; 
    myProbe.lin->Channel = 0;
    myProbe.lin->currAngle = 0;
    myProbe.lin->homeAngle = 0;

    myProbe.rot->Channel = 15;
    myProbe.rot->currAngle = 0;
    myProbe.rot->homeAngle = 0;

    myProbe.nema->currAngle = HOME.x;
    myProbe.nema->homeAngle = HOME.x;
	
	current_state = State_Listen;

	while(1){
		flashLED(LD1_GPIO_Port, LD1_Pin, 500, 1);

		if (current_state != NULL) {
            current_state();       // Run the state logic
        }
	}

}
