#ifndef __ADC_FFT_H__
#define __ADC_FFT_H__


#include "config.h"
#include "main.h"

#ifndef __FPU_PRESENT
#define __FPU_PRESENT 1U
#endif

#define ARM_MATH_CM4
#include "arm_math.h"
#define ADC_BUF_LEN 2048
#define FFT_BUFFER_SIZE 2048
#define SAMPLE_RATE_HZ 1000000 //based on the ADC timer interrupt - might need to measure real ADC speed

extern uint16_t adc_buffer[ADC_BUF_LEN];
extern uint8_t ADC_full;
extern uint8_t sendFFT_ready;


extern float input_FFT[FFT_BUFFER_SIZE];
extern float output_FFT[FFT_BUFFER_SIZE];


void computeCoeffs(float output_FFT[FFT_BUFFER_SIZE]);

void sendADC_UART();

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);


void sendFFT_UART();


#endif /* __ADC_FFT_H__ */
