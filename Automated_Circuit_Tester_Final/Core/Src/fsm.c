#include "config.h"
#include "fsm.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>

static const uint8_t CMD_TAKEPIC[] = "TAKEPC\r\n";
static const uint8_t CMD_ADCFFT[]  = "ADCFFT\r\n";
static const uint8_t CMD_COORDS[]  = "COORDS\r\n";
static const uint8_t CMD_MOTORS[]  = "MOTORS\r\n";

static const uint8_t CMD_GO[]      = "GOMOTO\r\n";    

static uint8_t rx_data_arr[25] = {0};


float xPos = 0.0f;
float yPos = 0.0f;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (!(huart->Instance == USART3)) {
        flashLED(LD3_GPIO_Port, LD3_Pin, 1000, 5);
    }


    switch (current_state) {
        case State_Listen:
            //since only one can match at a time, we can simply compare sequentially
            if (match_command(rx_data_arr, CMD_TAKEPIC)) {
                current_state = State_Picture;

            } else if (match_command(rx_data_arr, CMD_ADCFFT)) {
                current_state = State_ADC_FFT;

            } else if (match_command(rx_data_arr, CMD_COORDS)) {
                current_state = State_Coordinate_RX;

            }else{
                flashLED(LD3_GPIO_Port, LD3_Pin, 500, 20);
                current_state = State_Listen; //no match

            }
            break;

        case State_Coordinate_RX: 
            //process floats 
            memcpy(&posX, &rx_data_arr[0], 4);  // First 4 bytes
            memcpy(&posY, &rx_data_arr[4], 4);  // Next 4 bytes

            assert_param(posX >= -20.0f && posX <= 20.0f && posY >= -20.0f && posY <= 20.0f); //make sure the position given is sane

            current_state = State_Motors;
            //current_state = State_WaitForGo;
            flashLED(LD2_GPIO_Port, LD2_Pin, 100, 3);
            
    }
}

void State_Listen(void){
    //State listen will blink LED1 and wait for UART communications to determine the next state to go into 
    //it will wait for certain preambles
    HAL_UART_Receive_IT(&huart3, rx_data_arr, 8);  //arm the interrupt for preamble
    while(1); //block till UART RX

}

void State_Motors(void){
    
}

void State_Coord_RX(void){
    HAL_UART_Receive_IT(&huart3, rx_data_arr, 8);  //arm the interrupt for 2 floats
    while(1); //block till UART RX
    
}

void State_WaitForGo(void){
    //basically a blocker to motor actuate until it receives a go. If receives anything other than go it indicates a problem and should force you to reset.
    while(1); //block till UART RX

}

void State_Picture(void){
    current_state = State_Listen;
}

void State_ADC_FFT(void){
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