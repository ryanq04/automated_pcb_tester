#include "main.h"
#include "motorposition.h"
#include "config.h"
#include "fsm.h"
#include "ov7670.h"
#include "adc_fft.h"
#include "camera_code.h"


#include <string.h>
#include <stdint.h>
#include <stddef.h>

static const uint8_t CMD_TAKEPIC[] = "TAKEPC\r\n";
static const uint8_t CMD_ADCFFT[]  = "ADCFFT\r\n";
static const uint8_t CMD_COORDS[]  = "COORDS\r\n";
static const uint8_t CMD_MOTORS[]  = "MOTORS\r\n";

static const uint8_t CMD_COORDS_RX[]  = "CRD_RX\r\n";
static const uint8_t CMD_GO[]      = "GOMOTO\r\n";    

static uint8_t rx_data_arr[25] = {0};

extern Probe myProbe;

float posX = 0.0f;
float posY = 0.0f;

extern arm_rfft_fast_instance_f32 fftHandler;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if (!(huart->Instance == USART3)) {
        return; // Not from USART3, ignore
    }

    switch (state) {
        case STATE_LISTEN:
            if (match_command(rx_data_arr, CMD_TAKEPIC)) {
                ptr_state = State_Picture; //next state 
            } else if (match_command(rx_data_arr, CMD_COORDS)) {
                ptr_state = State_Coord_RX;
            } else if (match_command(rx_data_arr, CMD_ADCFFT)) {

                ptr_state = State_ADC_FFT;
                HAL_UART_Transmit(&huart3, CMD_ADCFFT, 8, 100);
            } else {

                ptr_state = State_Listen;
            }
            break;

        case STATE_COORDS:
        	
            memcpy(&posX, &rx_data_arr[0], 4);
            memcpy(&posY, &rx_data_arr[4], 4);
            ptr_state = State_Motors;

//            posX = 1.0;
//            posY = 1.0;
//
//            HAL_UART_Transmit(&huart3, (uint8_t*) &posX, 4, 100);
//            HAL_UART_Transmit(&huart3, (uint8_t*) &posY, 4, 100);

            HAL_UART_Transmit(&huart3, rx_data_arr, 8, 100); //return received coords
            break;

        default:
            // Unexpected state in UART callback
        	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
            state = STATE_LISTEN;
            ptr_state = State_Listen;
            break;
    }
}

void State_Listen(void){
    state = STATE_LISTEN;
    ptr_state = NULL;
    //flashLED(LD1_GPIO_Port, LD1_Pin, 100, 5);
    //State listen will blink LED1 and wait for UART communications to determine the next state to go into 
    //it will wait for certain preambles
    HAL_UART_Receive_IT(&huart3, rx_data_arr, 8);  //arm the interrupt for preamble
}

void State_Picture(void){
    HAL_UART_Transmit(&huart3, CMD_TAKEPIC, 8, 100); //ack state entry
    state = STATE_TAKEPIC;
    p3();
    ptr_state = State_Listen;
}

void State_Coord_RX(void){
    state = STATE_COORDS;
    ptr_state = NULL;
    HAL_UART_Receive_IT(&huart3, rx_data_arr, 8);  //arm the interrupt for 2 floats
    HAL_UART_Transmit(&huart3, CMD_COORDS, 8, 100); //ack ready for coords
}

void State_Motors(void){
    state = STATE_MOTORS;
    init_home(&myProbe);
    Position test = {posX, posY, 0.0};
    moveProbe_test(&myProbe, test);
    ptr_state = State_Listen;
}

void State_WaitForGo(void){
    state = STATE_GO;
    //basically a blocker to motor actuate until it receives a go. If receives anything other than go it indicates a problem and should force you to reset.
    ptr_state = NULL;
}


void State_ADC_FFT(void){
    // state = STATE_ADCFFT;
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUF_LEN); //start DMA and ADC
	// HAL_TIM_Base_Start(&htim2);  // Start the timer that triggers ADC
	// //data is ready for FFT
    // while(1){
    //     if(ADC_full == 1){
    //         ADC_full = 0;
    //         HAL_ADC_Stop_DMA(&hadc1);
    //         HAL_TIM_Base_Stop(&htim2); // stop the adc and timer

    //         for(int i = 0; i < ADC_BUF_LEN; i++){ //since DMA is faster than code, we should be able to immediately load values
	// 		    input_FFT[i] = (float)(adc_buffer[i]); //note the usage of float here - should consider optimization reasons and configurations
	// 	    }

    //         //FFT
    //         arm_rfft_fast_f32(&fftHandler, input_FFT, output_FFT, 0);
    //         computeCoeffs(output_FFT);

    //         sendADC_UART();
    //         sendFFT_UART();
    //     }
    // }
    // ptr_state = State_Listen;
}


/**
 * @brief Compare fixed-length 8-byte command strings.
 * @param input     Pointer to received buffer (must be at least 8 bytes)
 * @param command   Pointer to expected command (8-byte string)
 * @return 1 if match, 0 if not
 */
uint8_t match_command(const uint8_t* input, const uint8_t* command) {
    return (memcmp(input, command, 8) == 0);
}
