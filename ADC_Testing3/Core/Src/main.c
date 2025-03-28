/*ATTENTION THIS CODE HAS ADC DEPENDENCIES IN HAL_MSP.C for configuring it to channel 0 
	-> HAL_ADC_MspInit
	-> FFT Lib in Lib
	-> arm_math.h in Inc
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "config.h"
#include <stdio.h>
#include <math.h>



#define ARM_MATH_CM4
#include "arm_math.h"

#define ADC_BUF_LEN 2048

#define FFT_BUFFER_SIZE 2048
#define SAMPLE_RATE_HZ 1000000 //based on the ADC timer interrupt - might need to measure real ADC speed
//timer is set to tick at 1 MHz but there are factors that affect whether this is accurate
/*
	1. trigger accuracy - clock dependent but good since PLL and crystal
	2. ADC conversion time: 12 bit res, 15 cycle sample, ADC clock timing. if timer period < ADC conversion time, samples will be missed. 

		SYSCLK: 168 MHz
		APB2 Prescaler:	/2
		PCLK2:	84 MHz
		ADC Prescaler:	/4 (via ADC_CLOCK_SYNC_PCLK_DIV4)
		ADC Clock:	84 MHz / 4 = 21 MHz

		Assuming this is 16 cycles per sample at 21 MHz we have a 738.1 ns ADC conversion time
		Theoretically, this means we have 1.355 MSps maximum

		I am setting my timer to be 1MHz -> 1MS ps requests, so the ADC should be able to keep up.
	3. DMA latency
*/


ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart3;
PCD_HandleTypeDef hpcd_USB_OTG_FS;


char msg[100];
uint16_t adc_res;
uint16_t adc_buffer[ADC_BUF_LEN];
uint8_t ADC_full = 0;
uint8_t sendFFT_ready = 0;



//FFT CODE
float input_FFT[FFT_BUFFER_SIZE];
float output_FFT[FFT_BUFFER_SIZE];

typedef struct FreqMagPair {
    float frequency;
    float magnitude;
} FreqMagPair;

FreqMagPair freq_plot[FFT_BUFFER_SIZE];





//get the frequency components and store them to reuse in the globals input_FFT (freq) and output_FFT (mag)
void computeCoeffs(float output_FFT[FFT_BUFFER_SIZE]) {
    for (int i = 0, fIndex = 0; i < FFT_BUFFER_SIZE; i += 2, fIndex++) {
        float real = output_FFT[i];
        float imag = output_FFT[i + 1];
        float mag = fabs(sqrtf(real * real + imag * imag));

        // Overwrite input/output FFT arrays with freq/mag
        input_FFT[fIndex] = ((float) fIndex * SAMPLE_RATE_HZ / (float) FFT_BUFFER_SIZE);
        output_FFT[fIndex] = mag;
    }
		
		sendFFT_ready = 1;
}


void sendADC_UART() {
    const char *preamble = "ADC\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t *)preamble, strlen(preamble), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart3, (uint8_t *)adc_buffer, sizeof(adc_buffer), HAL_MAX_DELAY);
}


void sendFFT_UART() {
		if(sendFFT_ready == 1){
			//note that this ensures that the input and output buffers are Hz/Mag after compute coeffs. 
			sendFFT_ready = 0;
			const char *preamble = "FFT\r\n";
			HAL_UART_Transmit(&huart3, (uint8_t *)preamble, strlen(preamble), HAL_MAX_DELAY);

			HAL_UART_Transmit(&huart3, (uint8_t *)input_FFT, (FFT_BUFFER_SIZE / 2) * sizeof(float), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart3, (uint8_t *)output_FFT, (FFT_BUFFER_SIZE / 2) * sizeof(float), HAL_MAX_DELAY);
		}
}


//FFT CODE INITS END



int main(void)
{


  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();

  //init FFT
  arm_rfft_fast_instance_f32 fftHandler;
  arm_rfft_fast_init_f32(&fftHandler, FFT_BUFFER_SIZE);



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
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   if (hadc->Instance == ADC1)
    {
      // adc_buffer is now full
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			ADC_full = 1;
    }

   


}


