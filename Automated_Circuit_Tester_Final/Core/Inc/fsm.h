#ifndef __FSM_H__
#define __FSM_H__

typedef enum {
    STATE_LISTEN,
    STATE_TAKEPIC,
    STATE_ADCFFT,
    STATE_COORDS,
    STATE_MOTORS,
    STATE_GO
} State;


typedef void (*StateFunc)(void);
extern StateFunc ptr_state;
extern State state;

void State_Listen(void);
void State_Motors(void);
void State_WaitForGo(void);
void State_Picture(void);
void State_ADC_FFT(void);
void State_Coord_RX(void);
uint8_t match_command(const uint8_t* input, const uint8_t* command);

#endif