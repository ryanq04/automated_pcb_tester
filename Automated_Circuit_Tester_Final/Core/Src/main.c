
#include "config.h"
#include "main.h"
#include <stdio.h>
#include <math.h>


#include "adc_fft.h"
#include "ov7670.h"

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

//camera globals
uint16_t snapshot_buff[IMG_ROWS * IMG_COLS] = {0};
uint8_t send_ptr[FRAMESIZE * 2] = {0};
uint8_t dma_flag = 0;

//fsm globals
StateFunc current_state = State_Listen;


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
	arm_rfft_fast_instance_f32 fftHandler;
	arm_rfft_fast_init_f32(&fftHandler, FFT_BUFFER_SIZE);



	//MAIN CONTROL LOOP
	while (1)
	{
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			HAL_Delay(100);
		if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)) {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			HAL_Delay(100);

			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUF_LEN); //start DMA and ADC
			HAL_TIM_Base_Start(&htim2);  // Start the timer that triggers ADC
			//data is ready for FFT
		}

		if(ADC_full == 1){
			ADC_full = 0;
			HAL_ADC_Stop_DMA(&hadc1);
			HAL_TIM_Base_Stop(&htim2); // stop the adc and timer

			for(int i = 0; i < ADC_BUF_LEN; i++){ //since DMA is faster than code, we should be able to immediately load values
				input_FFT[i] = (float)(adc_buffer[i]); //note the usage of float here - should consider optimization reasons and configurations
			}

			//FFT
			arm_rfft_fast_f32(&fftHandler, input_FFT, output_FFT, 0);
			computeCoeffs(output_FFT);

			sendADC_UART();
			sendFFT_UART();
		}
	}

  /*
	while(1){
		assert_param(current_state != NULL);
        current_state();  // Call the current state function
        
	
	}
 
  */
}
