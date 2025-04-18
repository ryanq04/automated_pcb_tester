
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



//camera globals
uint16_t snapshot_buff[IMG_ROWS * IMG_COLS] = {0}; // 25056
// uint8_t send_ptr[FRAMESIZE * 2] = {0};
uint8_t dma_flag = 0;

//ADC_FFT globals
uint16_t adc_buffer[ADC_BUF_LEN]; // bufflen = 2048
uint8_t ADC_full = 0;
uint8_t sendFFT_ready = 0;
float input_FFT[FFT_BUFFER_SIZE];
float output_FFT[FFT_BUFFER_SIZE];
arm_rfft_fast_instance_f32 fftHandler;

//fsm globals
StateFunc ptr_state = NULL;
State state = STATE_LISTEN;


//init motor globals
Probe myProbe; 
Servo lin;
Servo rot;
Stepper nema;

const Position HOME = {6.0, 6.0, 12.2};
	

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


	//init PCA
	PCA9685_Init(50); // 50Hz for servo
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

    myProbe.nema->currAngle = 0;
	
	ptr_state = State_Listen;

	__HAL_DMA_ENABLE_IT(&hdma_dcmi, DMA_IT_TC);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	ov7670_init();
	ov7_config();

	while(1){

		if (ptr_state != NULL) {
            ptr_state();       // Run the state logic
        }
	}

}
