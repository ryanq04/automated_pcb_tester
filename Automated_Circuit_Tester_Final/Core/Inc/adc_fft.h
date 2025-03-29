

#define ARM_MATH_CM4
#include "arm_math.h"
#define ADC_BUF_LEN 2048
#define FFT_BUFFER_SIZE 2048
#define SAMPLE_RATE_HZ 1000000 //based on the ADC timer interrupt - might need to measure real ADC speed