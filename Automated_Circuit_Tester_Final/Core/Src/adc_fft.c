#include "adc_fft.h"


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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   if (hadc->Instance == ADC1)
    {
      // adc_buffer is now full
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			ADC_full = 1;
    }
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
