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
        flashLED(LD3_GPIO_Port, LD3_Pin, 1000, 5);
    }

    if (current_state == State_Listen) {
        
        if (match_command(rx_data_arr, CMD_TAKEPIC)) {
        	flashLED(LD2_GPIO_Port, LD2_Pin, 100, 10);
            current_state = State_Picture; //next state logic
            HAL_UART_Transmit(&huart3, CMD_TAKEPIC, 8, 100); //ack

        } else if (match_command(rx_data_arr, CMD_ADCFFT)) {
            current_state = State_ADC_FFT;
            HAL_UART_Transmit(&huart3, CMD_ADCFFT, 8, 100);

        } else if (match_command(rx_data_arr, CMD_COORDS)) {
            current_state = State_Coord_RX;
            HAL_UART_Transmit(&huart3, CMD_COORDS, 8, 100);

        } else { // No match
            flashLED(LD3_GPIO_Port, LD3_Pin, 500, 20);
            current_state = State_Listen; 
        }

    //end Listen transiitons   

    //Waiting for coordinate receive state:
    } else if (current_state == State_Coord_RX) {
        // Process floats 
        memcpy(&posX, &rx_data_arr[0], 4);  // First 4 bytes
        memcpy(&posY, &rx_data_arr[4], 4);  // Next 4 bytes

        assert_param(posX >= -20.0f && posX <= 20.0f && posY >= -20.0f && posY <= 20.0f); // bounded -20cm , 20cm

        current_state = State_Motors;
        // current_state = State_WaitForGo; // Normally what we should do

        flashLED(LD2_GPIO_Port, LD2_Pin, 100, 3);
        HAL_UART_Transmit(&huart3, CMD_COORDS_RX, 8, 100);  // Ack
    }


    current_state(); //go to next state
}

void State_Listen(void){
    current_state = NULL;
    flashLED(LD1_GPIO_Port, LD1_Pin, 50, 5);
    //State listen will blink LED1 and wait for UART communications to determine the next state to go into 
    //it will wait for certain preambles
    HAL_UART_Receive_IT(&huart3, rx_data_arr, 8);  //arm the interrupt for preamble
}

void State_Picture(void){
    p3();
    current_state = State_Listen;
}

void State_Coord_RX(void){
    current_state = NULL;
    HAL_UART_Receive_IT(&huart3, rx_data_arr, 8);  //arm the interrupt for 2 floats
    
}

void State_Motors(void){
    init_home(&myProbe);
    Position test = {3, 3, 0};
    moveProbe_test(&myProbe, test);
    flashLED(LD2_GPIO_Port, LD2_Pin, 100, 5);
    current_state = State_Listen;
}

void State_WaitForGo(void){
    //basically a blocker to motor actuate until it receives a go. If receives anything other than go it indicates a problem and should force you to reset.
    current_state = NULL;
}


void State_ADC_FFT(void){
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUF_LEN); //start DMA and ADC
	HAL_TIM_Base_Start(&htim2);  // Start the timer that triggers ADC
	//data is ready for FFT
    while(1){
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
    current_state = State_Listen;
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
